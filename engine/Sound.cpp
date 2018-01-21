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

void ok::sound::SoundRoom::Listen(sf::Sound & sound, glm::vec3 room_coordinates)
{
	//sound.setp
}
