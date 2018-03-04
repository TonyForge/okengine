#include "Sound.h"

ok::sound::SoundInstance* ok::sound::SoundInstance::_injection_safety_lock = nullptr;

ok::Pool<ok::sound::SampleWrapper> ok::sound::Sound::_sample_slots(
	256-16,
	[]() //allocator
	{
		return new ok::sound::SampleWrapper(nullptr, new ok::sound::Sample());
	},
	[](ok::sound::SampleWrapper& input) //injecter
	{
		if (input.instance != nullptr)
		{
			input.instance->_Inject();
			input.instance = nullptr;
		}

		input.sample->stop();
		input.sample->resetBuffer();
	}
);

ok::Pool<ok::sound::StreamWrapper> ok::sound::Sound::_stream_slots(
	16,
	[]() //allocator
	{
		return new ok::sound::StreamWrapper(nullptr, new ok::sound::Stream());
	},
	[](ok::sound::StreamWrapper& input) //injecter
	{
		if (input.instance != nullptr)
		{
			input.instance->_Inject();
			input.instance = nullptr;
		}

		input.stream->stop();
	}
);

ok::sound::Sound::Sound(ok::sound::SoundAsset* asset, int instances_limit) : 
	_instances(
		instances_limit,
		nullptr,
		[](ok::sound::SoundInstance& input) //injecter
		{
			if (&input != ok::sound::SoundInstance::_injection_safety_lock)
			{
				if (input._sample_slot != nullptr)
				{
					(**input._sample_slot).instance = nullptr;
					ok::sound::Sound::_sample_slots.Inject(input._sample_slot);
				}
				if (input._stream_slot != nullptr)
				{
					(**input._stream_slot).instance = nullptr;
					ok::sound::Sound::_stream_slots.Inject(input._stream_slot);
				}
			}

			input._sample_slot = nullptr;
			input._stream_slot = nullptr;
		}
	)
{
	_asset = asset;
	_setup_instance = nullptr;

	_setup_paused = false;
	_setup_stopped = false;
	_setup_looped = false;
	_setup_muted = false;
	_setup_volume = 1.f;
}

void ok::sound::Sound::SetupBegin(ok::sound::SoundInstance * instance)
{
	_setup_instance = instance;
}

void ok::sound::Sound::SetupEnd()
{
	_setup_instance = nullptr;
}

ok::sound::SoundInstance * ok::sound::Sound::Play()
{
	std::shared_ptr<ok::PoolContainer<ok::sound::SoundInstance>> instance_container = _instances.Eject();
	ok::sound::SoundInstance& instance = **instance_container;

	if (_asset->IsSample())
	{
		instance._sample_slot = _sample_slots.Eject();
		ok::sound::Sample& sample = *(**instance._sample_slot).sample;
		_asset->Apply(sample);

		if (_setup_paused || _setup_stopped)
		{
			//do nothing
		}
		else sample.play();
	}

	if (_asset->IsStream())
	{
		instance._stream_slot = _stream_slots.Eject();
		ok::sound::Stream& stream = *(**instance._stream_slot).stream;
		_asset->Apply(stream);

		if (_setup_paused || _setup_stopped)
		{
			//do nothing
		}
		else stream.play();
	}

	instance._parent_pool = &_instances;
	instance._parent_pool_container = instance_container;

	SetupBegin(&instance);
		if (_setup_looped) EnableLoop();
		if (_setup_muted) EnableMute();
		SetVolume(_setup_volume);
	SetupEnd();

	return &instance;
}

void ok::sound::Sound::Pause()
{
	if (_setup_instance == nullptr)
	{
		_setup_paused = true;

		auto& items = _instances.Items();

		for (auto& item : items)
		{
			if (item->IsOut())
			{
				ok::sound::SoundInstance& instance = **item;
				if (instance._sample_slot) (**instance._sample_slot).sample->pause();
				if (instance._stream_slot) (**instance._stream_slot).stream->pause();
			}
		}
	}
	else
	{
		ok::sound::SoundInstance& instance = *_setup_instance;
		if (instance._sample_slot) (**instance._sample_slot).sample->pause();
		if (instance._stream_slot) (**instance._stream_slot).stream->pause();
	}
}

void ok::sound::Sound::Resume()
{
	if (_setup_instance == nullptr)
	{
		_setup_paused = false;
		_setup_stopped = false;

		auto& items = _instances.Items();

		for (auto& item : items)
		{
			if (item->IsOut())
			{
				ok::sound::SoundInstance& instance = **item;
				if (instance._sample_slot) (**instance._sample_slot).sample->play();
				if (instance._stream_slot) (**instance._stream_slot).stream->play();
			}
		}
	}
	else
	{
		ok::sound::SoundInstance& instance = *_setup_instance;
		if (instance._sample_slot) (**instance._sample_slot).sample->play();
		if (instance._stream_slot) (**instance._stream_slot).stream->play();
	}
}

void ok::sound::Sound::Stop()
{
	if (_setup_instance == nullptr)
	{
		_setup_stopped = true;

		auto& items = _instances.Items();

		for (auto& item : items)
		{
			if (item->IsOut())
			{
				ok::sound::SoundInstance& instance = **item;
				if (instance._sample_slot) (**instance._sample_slot).sample->stop();
				if (instance._stream_slot) (**instance._stream_slot).stream->stop();
			}
		}
	}
	else
	{
		ok::sound::SoundInstance& instance = *_setup_instance;
		if (instance._sample_slot) (**instance._sample_slot).sample->stop();
		if (instance._stream_slot) (**instance._stream_slot).stream->stop();
	}
}

void ok::sound::Sound::EnableLoop()
{
	if (_setup_instance == nullptr)
	{
		_setup_looped = true;

		auto& items = _instances.Items();

		for (auto& item : items)
		{
			if (item->IsOut())
			{
				ok::sound::SoundInstance& instance = **item;
				if (instance._sample_slot) (**instance._sample_slot).sample->setLoop(true);
				if (instance._stream_slot) (**instance._stream_slot).stream->setLoop(true);
			}
		}
	}
	else
	{
		ok::sound::SoundInstance& instance = *_setup_instance;
		if (instance._sample_slot) (**instance._sample_slot).sample->setLoop(true);
		if (instance._stream_slot) (**instance._stream_slot).stream->setLoop(true);
	}
}

void ok::sound::Sound::DisableLoop()
{
	if (_setup_instance == nullptr)
	{
		_setup_looped = false;

		auto& items = _instances.Items();

		for (auto& item : items)
		{
			if (item->IsOut())
			{
				ok::sound::SoundInstance& instance = **item;
				if (instance._sample_slot) (**instance._sample_slot).sample->setLoop(false);
				if (instance._stream_slot) (**instance._stream_slot).stream->setLoop(false);
			}
		}
	}
	else
	{
		ok::sound::SoundInstance& instance = *_setup_instance;
		if (instance._sample_slot) (**instance._sample_slot).sample->setLoop(false);
		if (instance._stream_slot) (**instance._stream_slot).stream->setLoop(false);
	}
}

void ok::sound::Sound::EnableMute()
{
	if (_setup_instance == nullptr)
	{
		_setup_muted = true;

		auto& items = _instances.Items();

		for (auto& item : items)
		{
			if (item->IsOut())
			{
				ok::sound::SoundInstance& instance = **item;
				instance._mute_enabled = true;
				if (instance._sample_slot) (**instance._sample_slot).sample->setVolume(0.f);
				if (instance._stream_slot) (**instance._stream_slot).stream->setVolume(0.f);
			}
		}
	}
	else
	{
		ok::sound::SoundInstance& instance = *_setup_instance;
		instance._mute_enabled = true;
		if (instance._sample_slot) (**instance._sample_slot).sample->setVolume(0.f);
		if (instance._stream_slot) (**instance._stream_slot).stream->setVolume(0.f);
	}
}

void ok::sound::Sound::DisableMute()
{
	if (_setup_instance == nullptr)
	{
		_setup_muted = false;

		auto& items = _instances.Items();

		for (auto& item : items)
		{
			if (item->IsOut())
			{
				ok::sound::SoundInstance& instance = **item;
				instance._mute_enabled = false;
				if (instance._sample_slot) (**instance._sample_slot).sample->setVolume(instance._volume*100.f);
				if (instance._stream_slot) (**instance._stream_slot).stream->setVolume(instance._volume*100.f);
			}
		}
	}
	else
	{
		ok::sound::SoundInstance& instance = *_setup_instance;
		instance._mute_enabled = false;
		if (instance._sample_slot) (**instance._sample_slot).sample->setVolume(instance._volume*100.f);
		if (instance._stream_slot) (**instance._stream_slot).stream->setVolume(instance._volume*100.f);
	}
}

void ok::sound::Sound::SetVolume(float volume)
{
	if (_setup_instance == nullptr)
	{
		_setup_volume = volume;

		auto& items = _instances.Items();

		for (auto& item : items)
		{
			if (item->IsOut())
			{
				ok::sound::SoundInstance& instance = **item;
				instance._volume = volume;
				if (instance._mute_enabled)
				{
					//do nothing
				}
				else
				{
					if (instance._sample_slot) (**instance._sample_slot).sample->setVolume(instance._volume*100.f);
					if (instance._stream_slot) (**instance._stream_slot).stream->setVolume(instance._volume*100.f);
				}
			}
		}
	}
	else
	{
		ok::sound::SoundInstance& instance = *_setup_instance;
		instance._volume = volume;
		if (instance._mute_enabled)
		{
			//do nothing
		}
		else
		{
			if (instance._sample_slot) (**instance._sample_slot).sample->setVolume(instance._volume*100.f);
			if (instance._stream_slot) (**instance._stream_slot).stream->setVolume(instance._volume*100.f);
		}
	}
}

void ok::sound::Sound::LinkToSpatialController(ok::Transform * transform, float* radius)
{
	if (_setup_instance == nullptr)
	{
		auto& items = _instances.Items();

		for (auto& item : items)
		{
			if (item->IsOut())
			{
				ok::sound::SoundInstance& instance = **item;
				instance._spatial_enabled = true;
				instance._spatial_controller_transform = transform;
				instance._spatial_controller_position = nullptr;
				instance._spatial_controller_radius = radius;
			}
		}
	}
	else
	{
		ok::sound::SoundInstance& instance = *_setup_instance;
		instance._spatial_enabled = true;
		instance._spatial_controller_transform = transform;
		instance._spatial_controller_position = nullptr;
		instance._spatial_controller_radius = radius;
	}
}

void ok::sound::Sound::LinkToSpatialController(glm::vec3 * position, float* radius)
{
	if (_setup_instance == nullptr)
	{
		auto& items = _instances.Items();

		for (auto& item : items)
		{
			if (item->IsOut())
			{
				ok::sound::SoundInstance& instance = **item;
				instance._spatial_enabled = true;
				instance._spatial_controller_transform = nullptr;
				instance._spatial_controller_position = position;
				instance._spatial_controller_radius = radius;
			}
		}
	}
	else
	{
		ok::sound::SoundInstance& instance = *_setup_instance;
		instance._spatial_enabled = true;
		instance._spatial_controller_transform = nullptr;
		instance._spatial_controller_position = position;
		instance._spatial_controller_radius = radius;
	}
}

void ok::sound::Sound::UnlinkFromSpatialController()
{
	if (_setup_instance == nullptr)
	{
		auto& items = _instances.Items();

		for (auto& item : items)
		{
			if (item->IsOut())
			{
				ok::sound::SoundInstance& instance = **item;
				instance._spatial_enabled = false;
				instance._spatial_controller_transform = nullptr;
				instance._spatial_controller_position = nullptr;
				instance._spatial_controller_radius = nullptr;
			}
		}
	}
	else
	{
		ok::sound::SoundInstance& instance = *_setup_instance;
		instance._spatial_enabled = false;
		instance._spatial_controller_transform = nullptr;
		instance._spatial_controller_position = nullptr;
		instance._spatial_controller_radius = nullptr;
	}
}

void ok::sound::Sound::EnableSpatial()
{
	if (_setup_instance == nullptr)
	{
		auto& items = _instances.Items();

		for (auto& item : items)
		{
			if (item->IsOut())
			{
				ok::sound::SoundInstance& instance = **item;
				instance._spatial_enabled = true;
			}
		}
	}
	else
	{
		ok::sound::SoundInstance& instance = *_setup_instance;
		instance._spatial_enabled = true;
	}
}

void ok::sound::Sound::DisableSpatial()
{
	if (_setup_instance == nullptr)
	{
		auto& items = _instances.Items();

		for (auto& item : items)
		{
			if (item->IsOut())
			{
				ok::sound::SoundInstance& instance = **item;
				instance._spatial_enabled = false;
			}
		}
	}
	else
	{
		ok::sound::SoundInstance& instance = *_setup_instance;
		instance._spatial_enabled = false;
	}
}

void ok::sound::Sound::Update(float dt)
{
	auto& items = _instances.Items();

	for (auto& item : items)
	{
		if (item->IsOut())
		{
			ok::sound::SoundInstance& instance = **item;

			if (instance.IsStopped() == true && instance.IsLooped() == false)
			{
				_instances.Inject(item);
				continue;
			}

			ok::sound::SoundRoom::o().Listen(instance);
		}
	}
}

ok::sound::SoundInstance::SoundInstance()
{
	_sample_slot = nullptr;
	_stream_slot = nullptr;
	_parent_pool = nullptr;
	_parent_pool_container = nullptr;
	_spatial_enabled = false;
	_spatial_controller_transform = nullptr;
	_spatial_controller_position = nullptr;
	_spatial_controller_radius = nullptr;

	_mute_enabled = false;
	_volume = 1.f;
}

bool ok::sound::SoundInstance::IsStopped()
{
	if (_sample_slot) return (**_sample_slot).sample->getStatus() == sf::SoundSource::Status::Stopped;
	if (_stream_slot) return (**_stream_slot).stream->getStatus() == sf::SoundSource::Status::Stopped;

	return false;
}

bool ok::sound::SoundInstance::IsLooped()
{
	if (_sample_slot) return (**_sample_slot).sample->getLoop();
	if (_stream_slot) return (**_stream_slot).stream->getLoop();

	return false;
}

void ok::sound::SoundInstance::_Inject()
{
	ok::sound::SoundInstance::_injection_safety_lock = this;
	_parent_pool->Inject(_parent_pool_container);
	ok::sound::SoundInstance::_injection_safety_lock = nullptr;
}

ok::sound::SampleWrapper::SampleWrapper(ok::sound::SoundInstance * _instance, ok::sound::Sample * _sample)
{
	sample = _sample;
	instance = _instance;
}

ok::sound::SampleWrapper::SampleWrapper()
{
	sample = new ok::sound::Sample();
	instance = nullptr;
}

ok::sound::SampleWrapper::~SampleWrapper()
{
	if (sample != nullptr)
	{
		sample->stop();
		sample->resetBuffer();
		delete sample;
	}
		
}

ok::sound::SampleWrapper & ok::sound::SampleWrapper::operator=(const ok::sound::SampleWrapper & right)
{
	sample = right.sample;
	instance = right.instance;

	return *this;
}

ok::sound::StreamWrapper::StreamWrapper(ok::sound::SoundInstance * _instance, ok::sound::Stream * _stream)
{
	stream = _stream;
	instance = _instance;
}

ok::sound::StreamWrapper::StreamWrapper()
{
	stream = new ok::sound::Stream();
	instance = nullptr;
}

ok::sound::StreamWrapper & ok::sound::StreamWrapper::operator=(const ok::sound::StreamWrapper & right)
{
	stream = right.stream;
	instance = right.instance;

	return *this;
}

ok::sound::StreamWrapper::~StreamWrapper()
{
	if (stream != nullptr)
	{
		stream->stop();
		delete stream;
	}
}

ok::sound::SoundRoom & ok::sound::SoundRoom::instance()
{
	static ok::sound::SoundRoom s;
	return s;
}

ok::sound::SoundRoom & ok::sound::SoundRoom::o()
{
	return instance();
}

void ok::sound::SoundRoom::LinkListener(ok::Transform* listener_transform, float* listener_radius)
{
	_listener_transform = listener_transform;
	_listener_radius = listener_radius;
}

void ok::sound::SoundRoom::Listen(ok::sound::SoundInstance & sound_instance)
{
	if (sound_instance._spatial_enabled)
	{
		glm::vec3 room_coordinates;

		if (sound_instance._spatial_controller_transform)
		{
			_listener_transform->BeginTransform(ok::TransformSpace::WorldSpace);
			sound_instance._spatial_controller_transform->BeginTransform(ok::TransformSpace::WorldSpace);

			room_coordinates = sound_instance._spatial_controller_transform->GetPosition() - _listener_transform->GetPosition();

			room_coordinates = glm::vec3(glm::dot(room_coordinates, _listener_transform->GetRight()), glm::dot(room_coordinates, _listener_transform->GetUp()), glm::dot(room_coordinates, _listener_transform->GetForward()));
			room_coordinates /= *_listener_radius;

			_listener_transform->EndTransform(false);
			sound_instance._spatial_controller_transform->EndTransform(false);
		}

		if (sound_instance._spatial_controller_position)
		{
			_listener_transform->BeginTransform(ok::TransformSpace::WorldSpace);

			room_coordinates = *sound_instance._spatial_controller_position - _listener_transform->GetPosition();

			room_coordinates = glm::vec3(glm::dot(room_coordinates, _listener_transform->GetRight()), glm::dot(room_coordinates, _listener_transform->GetUp()), glm::dot(room_coordinates, _listener_transform->GetForward()));
			room_coordinates /= *_listener_radius;

			_listener_transform->EndTransform(false);
		}

		float room_to_sfml_scale = 1000.f*(room_coordinates.length() / glm::clamp(*sound_instance._spatial_controller_radius / *_listener_radius, 0.001f, 1000.0f));

		if (sound_instance._sample_slot != nullptr)
		{
			ok::sound::SampleWrapper& sample = **sound_instance._sample_slot;

			sample.sample->setRelativeToListener(false);
			sample.sample->setMinDistance(1.f);
			sample.sample->setAttenuation(1.f);

			sample.sample->setPosition(room_coordinates.x * room_to_sfml_scale, room_coordinates.y * room_to_sfml_scale, room_coordinates.z * room_to_sfml_scale);
		}

		if (sound_instance._stream_slot != nullptr)
		{
			ok::sound::StreamWrapper& stream = **sound_instance._stream_slot;

			stream.stream->setRelativeToListener(false);
			stream.stream->setMinDistance(1.f);
			stream.stream->setAttenuation(1.f);

			stream.stream->setPosition(room_coordinates.x * room_to_sfml_scale, room_coordinates.y * room_to_sfml_scale, room_coordinates.z * room_to_sfml_scale);
		}
	}
	else
	{
		if (sound_instance._sample_slot != nullptr)
		{
			ok::sound::SampleWrapper& sample = **sound_instance._sample_slot;
			sample.sample->setRelativeToListener(true);
			sample.sample->setPosition(0, 0, 0);
			sample.sample->setMinDistance(1.f);
			sample.sample->setAttenuation(1.f);
		}
		if (sound_instance._stream_slot != nullptr)
		{
			ok::sound::StreamWrapper& stream = **sound_instance._stream_slot;
			stream.stream->setRelativeToListener(true);
			stream.stream->setPosition(0, 0, 0);
			stream.stream->setMinDistance(1.f);
			stream.stream->setAttenuation(1.f);
		}
	}
}
