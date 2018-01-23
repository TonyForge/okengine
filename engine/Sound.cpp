#include "Sound.h"

ok::sound::Man & ok::sound::Man::instance()
{
	static ok::sound::Man s;
	return s;
}

ok::sound::Man & ok::sound::Man::o()
{
	return instance();
}

ok::sound::SoundID & ok::sound::SoundID::operator=(const ok::sound::SoundID & right)
{
	id = right.id;
	type = right.type;
	return *this;
}

bool ok::sound::SoundID::operator==(const ok::sound::SoundID & right)
{
	return id == right.id;
}

glm::vec3 ok::sound::SoundRoom::GetRoomCoordinates(ok::Transform & listener, glm::vec3 sound_source_world_position, float listener_radius)
{
	listener.BeginTransform(ok::TransformSpace::WorldSpace);

	glm::vec3 source_location = sound_source_world_position - listener.GetPosition();

	source_location = glm::vec3(glm::dot(source_location, listener.GetRight()), glm::dot(source_location, listener.GetUp()), glm::dot(source_location, listener.GetForward()));
	source_location /= listener_radius;

	listener.EndTransform();

	return source_location;
}

glm::vec3 ok::sound::SoundRoom::GetRoomCoordinates(ok::Transform & listener, ok::Transform & sound_source, float listener_radius)
{
	listener.BeginTransform(ok::TransformSpace::WorldSpace);
	sound_source.BeginTransform(ok::TransformSpace::WorldSpace);

	glm::vec3 source_location = sound_source.GetPosition() - listener.GetPosition();

	source_location = glm::vec3(glm::dot(source_location, listener.GetRight()), glm::dot(source_location, listener.GetUp()), glm::dot(source_location, listener.GetForward()));
	source_location /= listener_radius;

	listener.EndTransform();
	sound_source.EndTransform();


	return source_location;
}

float ok::sound::SoundRoom::GetSoundSourceRoomRadius(float listener_radius, float sound_source_radius)
{
	return sound_source_radius / listener_radius;
}

void ok::sound::SoundRoom::Listen(sf::Sound & sound, glm::vec3 room_coordinates, float sound_source_room_radius)
{
	sound.setRelativeToListener(false);
	sound.setMinDistance(1.f);
	sound.setAttenuation(1.f);

	float room_to_sfml_scale = 1000.f*(room_coordinates.length() / glm::clamp(sound_source_room_radius, 0.001f,1000.0f));

	sound.setPosition(room_coordinates.x * room_to_sfml_scale, room_coordinates.y * room_to_sfml_scale, room_coordinates.z * room_to_sfml_scale);
}

void ok::sound::SoundRoom::Listen(sf::Music & music, glm::vec3 room_coordinates, float sound_source_room_radius)
{
	music.setRelativeToListener(false);
	music.setMinDistance(1.f);
	music.setAttenuation(1.f);

	float room_to_sfml_scale = 1000.f*(room_coordinates.length() / glm::clamp(sound_source_room_radius, 0.001f, 1000.0f));

	music.setPosition(room_coordinates.x * room_to_sfml_scale, room_coordinates.y * room_to_sfml_scale, room_coordinates.z * room_to_sfml_scale);
}

void ok::sound::SoundRoom::Listen(sf::Sound & sound)
{
	sound.setRelativeToListener(true);
	sound.setPosition(0, 0, 0);
	sound.setMinDistance(1.f);
	sound.setAttenuation(1.f);
}

void ok::sound::SoundRoom::Listen(sf::Music & music)
{
	music.setRelativeToListener(true);
	music.setPosition(0, 0, 0);
	music.setMinDistance(1.f);
	music.setAttenuation(1.f);
}

ok::sound::SFSoundWrapper::SFSoundWrapper() : sf::Sound()
{
	Unlink();
}

void ok::sound::SFSoundWrapper::LinktTo(ok::Transform * transform)
{
	_transform = transform;
	_position = nullptr;
}

void ok::sound::SFSoundWrapper::LinktTo(glm::vec3 * position)
{
	_transform = nullptr;
	_position = position;
}

void ok::sound::SFSoundWrapper::Unlink()
{
	_transform = nullptr;
	_position = nullptr;
}

ok::sound::SFMusicWrapper::SFMusicWrapper() : sf::Music()
{
	Unlink();
}

void ok::sound::SFMusicWrapper::LinktTo(ok::Transform * transform)
{
	_transform = transform;
	_position = nullptr;
}

void ok::sound::SFMusicWrapper::LinktTo(glm::vec3 * position)
{
	_transform = nullptr;
	_position = position;
}

void ok::sound::SFMusicWrapper::Unlink()
{
	_transform = nullptr;
	_position = nullptr;
}

ok::sound::SoundChannel::SoundChannel()
{
	_sound_ptr = nullptr;
	_music_ptr = nullptr;
}

bool ok::sound::SoundChannel::IsValid()
{
	if (_sound_ptr != nullptr && _sound_ptr->_owner == this) return true;
	if (_music_ptr != nullptr && _music_ptr->_owner == this) return true;
	return false;
}

void ok::sound::SoundChannel::_Play()
{
	if (_sound_ptr) _sound_ptr->play();
	if (_music_ptr) _music_ptr->play();
}

void ok::sound::SoundChannel::_Pause()
{
	if (_sound_ptr) _sound_ptr->pause();
	if (_music_ptr) _music_ptr->pause();
}

void ok::sound::SoundChannel::_Stop()
{
	if (_sound_ptr) _sound_ptr->stop();
	if (_music_ptr) _music_ptr->stop();
}

void ok::sound::Man::Update(float dt)
{
	_channels.Update(dt);
	_sounds.Update(dt);
	_musics.Update(dt);
}
