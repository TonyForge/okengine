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
