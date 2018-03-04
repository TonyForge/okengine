#include "Starpom.h"

#ifdef STARPOM

INITIALIZE_EASYLOGGINGPP

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Starpom::App app;

	LOG(INFO) << "Starpom initiated...";
	app.Run();

	return 0;
}

Starpom::App::App()
{
	_settings_file_name = "starpom.settings.xml";
}

Starpom::SS_ShipAgent* ss_ship_agent_001 = nullptr;
ok::graphics::Camera* camera;

void Starpom::App::Init()
{
	camera = new ok::graphics::Camera(ok::graphics::CameraCoordinateSystem::ScreenCenter);

	camera->SetProjectionOrtho(static_cast<float>(screen_width), static_cast<float>(screen_height), 1.f, 10000.f);
	camera->BeginTransform();
	camera->SetPosition(glm::vec3(0.f,0.f,-1000.f));
	camera->EndTransform(false);

	camera->LookAt(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
}


void Starpom::App::Update(float dt)
{
	if (Starpom::Loading::_instance == nullptr && Starpom::SS_ShipAgent::blueprints.size() == 0)
	{
		//load blueprints
		new Starpom::Loading(new Starpom::Task_SS_Blueprints());
	}

	if (Starpom::Loading::_instance == nullptr)
	{
		//do nothing
		if (ss_ship_agent_001 == nullptr)
		{
			Starpom::SS_ShipAgent* blueprint = Starpom::SS_ShipAgent::blueprints["nomad"];
			ss_ship_agent_001 = static_cast<Starpom::SS_ShipAgent*>(blueprint->Duplicate());
			ss_ship_agent_001->UpdateAbsoluteTransform(true);
		}
		else
		{
			ok::graphics::Camera::PushCamera(camera);

			ss_ship_agent_001->BeginTransform();
			ss_ship_agent_001->SetScale(glm::vec3(2.0f, 2.0f, 2.0f));
			ss_ship_agent_001->SetRotation(ss_ship_agent_001->GetRotation() + glm::vec3(dt*10.0f, dt*10.0f, dt*10.0f));
			ss_ship_agent_001->EndTransform(true);
			//ss_ship_agent_001->UpdateAbsoluteTransform();

			ss_ship_agent_001->Update(dt);
			ok::graphics::LayeredRenderer::instance().Flush();

			ok::graphics::Camera::PopCamera();
		}
	}
	else
	{
		if (Starpom::Loading::_instance->Update(dt))
		{
			delete Starpom::Loading::_instance;
			Starpom::Loading::_instance = nullptr;
		}
	}
}

#endif