#include "Game.h"

Treasurer::Game::Game()
{
	_settings_file_name = "treasurer.settings.xml";
}

void Treasurer::Game::Init()
{
	DisableFeature(ok::ApplicationFeature::AutoClearStencil);

	camera = new ok::graphics::Camera(ok::graphics::CameraCoordinateSystem::CartesianCenter);
	camera->SetProjectionOrtho(static_cast<float>(ok::Application::screen_width), static_cast<float>(ok::Application::screen_height), 1.f, 1000.f);
	
	camera->BeginTransform();
	camera->SetPosition(glm::vec3(500.f, 500.f, 500.f));
	camera->EndTransform(true);

	camera->LookAt(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 1.f));

	tank = new ok::Treasurer::PlayerNode();
}

void Treasurer::Game::Update(float dt)
{
	

	ok::graphics::Camera::PushCamera(camera);

	//tile_map_editor.Update(dt);
	tank->Update(dt);

	ok::graphics::Camera::PopCamera();
}