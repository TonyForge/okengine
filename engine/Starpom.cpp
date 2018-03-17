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

Starpom::Spaceship* go_spaceship = nullptr;
Starpom::SS_ShipAgent* ss_ship_agent_001 = nullptr;
ok::graphics::Camera* camera;
ok::Transform* camera_transform;
Starpom::Actions actions;
ok::graphics::LineBatch* line_batch;

void Starpom::App::Init()
{
	DisableFeature(ok::ApplicationFeature::AutoClearStencil);

	camera = new ok::graphics::Camera(ok::graphics::CameraCoordinateSystem::ScreenCenter);

	camera->SetProjectionOrtho(static_cast<float>(screen_width), static_cast<float>(screen_height), 1.f, 10000.f);
	//camera->SetProjectionPersp(static_cast<float>(screen_width), static_cast<float>(screen_height), 45.f, 1.f, 10000.f);
	camera->BeginTransform();
	camera->SetPosition(glm::vec3(0.f, 0.f, -1000.f));
	camera->EndTransform(false);

	camera_transform = new ok::Transform();
	camera_transform->AddChild(camera);

	Starpom::Actions::SetInstance(&actions);

	line_batch = new ok::graphics::LineBatch();
}


glm::vec3 dpos[3];
int pos_taken = 0;

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
		Starpom::Actions::o()._pause = false;
		Starpom::Actions::o().Update(dt);
		
		if (ss_ship_agent_001 == nullptr)
		{
			Starpom::SS_ShipAgent* blueprint = Starpom::SS_ShipAgent::blueprints["nomad"];
			ss_ship_agent_001 = static_cast<Starpom::SS_ShipAgent*>(blueprint->Duplicate());
			ss_ship_agent_001->UpdateAbsoluteTransform(true);

			ss_ship_agent_001->actions = Starpom::Actions::o().EjectOwner();

			go_spaceship = new Starpom::Spaceship();
			go_spaceship->ss_agent = blueprint;

			
			(**(ss_ship_agent_001->actions))._actions_continious.push_back(new Starpom::SS_ShipAgent::Action_FlyInSS(go_spaceship->ss_agent));

			Starpom::SS_ShipAgent::Action_FlyInSS* action = static_cast<Starpom::SS_ShipAgent::Action_FlyInSS*>(*((**(ss_ship_agent_001->actions))._actions_continious.begin()));
			action->path.BeginWaypointsCollection();

			float scl = 1.0f;

			action->path.CollectWaypoint(glm::vec3(0.f, 0.f, 0.f)*scl);
			action->path.CollectWaypoint(glm::vec3(100.f, 0.f, 0.f)*scl);
			action->path.CollectWaypoint(glm::vec3(200.f, 100.f, 0.f)*scl);
			action->path.CollectWaypoint(glm::vec3(200.f, 200.f, 0.f)*scl);
			action->path.EndWaypointsCollection(25.0f*scl);
		}
		else
		{
			ok::graphics::Camera::PushCamera(camera);

			camera_transform->BeginTransform();
			//camera_transform->SetRotation(camera_transform->GetRotation() + dt*glm::vec3(0.f, 10.f, 0.f));
			camera_transform->EndTransform(true);
			/*ss_ship_agent_001->BeginTransform();
			ss_ship_agent_001->SetScale(glm::vec3(0.35f, 0.35f, 0.35f));
			ss_ship_agent_001->SetRotation(ss_ship_agent_001->GetRotation() + glm::vec3(dt*10.0f, dt*10.0f, dt*10.0f));
			ss_ship_agent_001->EndTransform(true);

			ss_ship_agent_001->Update(dt);*/

			go_spaceship->Update(dt);

			
			
			Starpom::SS_ShipAgent::Action_FlyInSS* action = static_cast<Starpom::SS_ShipAgent::Action_FlyInSS*>(*((**(ss_ship_agent_001->actions))._actions_continious.begin()));
			ok::graphics::LayeredRenderer::instance().Flush();


			Starpom::SmoothPath::DrawDebug(*line_batch, action->path);

			if (ok::Input::o().KeyPressed(ok::MKey::Left))
			{
				dpos[pos_taken] = glm::vec3(-1024.0f*0.5f + ok::Input::o().MousePXPY().x, -768.0f*0.5f + ok::Input::o().MousePXPY().y, 0.f);
				pos_taken++;
				if (pos_taken == 3)
				{
					Starpom::SS_ShipAgent::Action_FlyInSS* action = static_cast<Starpom::SS_ShipAgent::Action_FlyInSS*>(*((**(ss_ship_agent_001->actions))._actions_continious.begin()));
					action->path.BeginWaypointsCollection();

					float scl = 1.0f;

					action->path.CollectWaypoint(dpos[0]*scl);
					action->path.CollectWaypoint(dpos[1] *scl);
					action->path.CollectWaypoint(dpos[2] *scl);

					action->path.EndWaypointsCollection(5.0f*scl);

					action->tick = 0;
					action->total_progress = 0;
					action->tick_progress = 0;

					pos_taken = 0;
				}
					
			}

			line_batch->SetBrushColor(ok::Color(1.f, 0.f, 1.f, 1.f));
			line_batch->BatchBegin();
			line_batch->MoveTo(dpos[0]);
			line_batch->LineTo(dpos[1]);
			line_batch->LineTo(dpos[2]);
			//line_batch->LineAB(glm::vec3(0.f, 0.f, 0.f), glm::vec3(-1024.0f*0.5f + ok::Input::o().MousePXPY().x, -768.0f*0.5f + ok::Input::o().MousePXPY().y, 0.f));
			line_batch->BatchEnd();

			/*line_batch->BatchBegin();

			line_batch->SetBrushColor(ok::Color(0.f, 1.f, 0.f, 1.f), ok::Color(1.f, 0.f, 0.f, 1.f));
			line_batch->SetBrushThickness(1.f);
			line_batch->SetBrushSoftness(0.010f);

			line_batch->LineAB(glm::vec3(0.f, 0.f, 0.f), glm::vec3(-1024.0f*0.5f+ok::Input::o().MousePXPY().x, -768.0f*0.5f + ok::Input::o().MousePXPY().y, 0.f));*/
			//line_batch->Circle(glm::vec3(-1024.0f*0.5f + ok::Input::o().MousePXPY().x, -768.0f*0.5f + ok::Input::o().MousePXPY().y, 0.f), camera->GetForward(), camera->GetUp(), 100.f, 5.f);

			//line_batch->BatchEnd();

			

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