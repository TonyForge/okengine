#include "Preloader.h"

void Zoner::Preloader::LoadItself()
{
	camera = new ok::graphics::Camera(ok::graphics::CameraCoordinateSystem::Screen);
	progress_bar = new ok::graphics::LineBatch(1);
}

void Zoner::Preloader::UnloadItself()
{
	delete camera;
	delete progress_bar;
}

void Zoner::Preloader::Task_DefaultResources()
{
	task = 0;
	task_step = 0;
	task_step_max = 1;

	//calculate steps count for default resources
	//open blueprints

}

void Zoner::Preloader::Update(float dt)
{
	ok::graphics::Camera::PushCamera(camera);
	ok::GameObject::Update(dt);

	if (task == 0)
	{
		_Task_DefaultResources();
	}

	float progress = static_cast<float>(task_step) / static_cast<float>(task_step_max);

	progress_bar->BatchBegin();
	progress_bar->MoveTo(glm::vec3(0.f, 0.f, 0.f));
	progress_bar->LineTo(glm::vec3())
	progress_bar->BatchEnd();

	ok::graphics::Camera::PopCamera();
}

void Zoner::Preloader::_Task_DefaultResources()
{
	if (task_step == 0)
	{
		//open blueprints
	}
}
