#include "ISpace.h"

std::vector<Zoner::IShip*> Zoner::ISpace::_who_is_there_container;

Zoner::ISpace::ISpace() : camera(ok::graphics::CameraCoordinateSystem::ScreenCenter)
{
	camera.SetProjectionOrtho(static_cast<float>(Zoner::IGame::o().GetScreenWidth()), static_cast<float>(Zoner::IGame::o().GetScreenHeight()), 1.0f, 1000.0f);

	camera.BeginTransform();
	camera.SetPosition(glm::vec3(0.f, 0.f, -500.0f));
	camera.EndTransform(true);

	isCurrent = false;
}
