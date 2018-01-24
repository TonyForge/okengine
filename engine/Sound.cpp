#include "Sound.h"



ok::sound::SoundInstance* ok::sound::Sound::_injection_safety_lock = nullptr;

ok::Pool<std::pair<ok::sound::SoundInstance*, ok::sound::Sample>> ok::sound::Sound::_sample_slots(
	256-16,
	[]() //allocator
	{
		return new std::pair<ok::sound::SoundInstance*, ok::sound::Sample>(nullptr, ok::sound::Sample());
	},
	[](std::pair<ok::sound::SoundInstance*, ok::sound::Sample>& input) //injector
	{
		if (input.first != nullptr)
		{
			ok::sound::Sound::_injection_safety_lock = input.first;
			input.first->_parent_pool->Inject(*(input.first->_parent_pool_container));
			ok::sound::Sound::_injection_safety_lock = nullptr;
			input.first = nullptr;
		}

		input.second.stop();
		input.second.resetBuffer();
	}
	);

ok::Pool<std::pair<ok::sound::SoundInstance*, ok::sound::Stream>> ok::sound::Sound::_stream_slots(
	16,
	[]() //allocator
	{
		return new std::pair<ok::sound::SoundInstance*, ok::sound::Stream>(nullptr, ok::sound::Stream());
	},
	[](std::pair<ok::sound::SoundInstance*, ok::sound::Stream>& input) //injector
	{
		if (input.first != nullptr)
		{
			ok::sound::Sound::_injection_safety_lock = input.first;
			input.first->_parent_pool->Inject(*(input.first->_parent_pool_container));
			ok::sound::Sound::_injection_safety_lock = nullptr;
			input.first = nullptr;
		}

		input.second.stop();
	}
	);

ok::sound::Sound::Sound(ok::sound::SoundAsset* asset, int instances_limit) : 
	_instances(
		instances_limit,
		nullptr,
		[](ok::sound::SoundInstance& input) //injector
		{
			if (&input != _injection_safety_lock)
			{
				if (input._sample_slot != nullptr)
				{
					(**input._sample_slot).first = nullptr;
					ok::sound::Sound::_sample_slots.Inject(*input._sample_slot);
				}
				if (input._stream_slot != nullptr)
				{
					(**input._stream_slot).first = nullptr;
					ok::sound::Sound::_stream_slots.Inject(*input._stream_slot);
				}
			}

			input._sample_slot = nullptr;
			input._stream_slot = nullptr;
		}
	)
{
	_asset = asset;
}

ok::sound::SoundInstance * ok::sound::Sound::Play()
{
	ok::sound::SoundInstance& instance = *_instances.Eject();

	if (_asset->IsSample())
	{
		instance._sample_slot = &_sample_slots.Eject();
		_asset->Apply((**instance._sample_slot).second);
	}

	if (_asset->IsStream())
	{
		instance._stream_slot = &_stream_slots.Eject();
		_asset->Apply((**instance._stream_slot).second);
	}

	return nullptr;
}

void ok::sound::Sound::Update(float dt)
{
	auto items = _instances.Items();
	ok::sound::SoundInstance* instance;

	for (auto item : items)
	{
		if (item.IsOut())
		{
			
		}
	}
}
