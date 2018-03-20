#include "Game.h"


Zoner::Game::Game()
{
	_settings_file_name = "zoner.settings.xml";

	memset(_game_states, false, sizeof(_game_states));

	Zoner::IGame::_instance = this;
}


void Zoner::Game::Init()
{
	DisableFeature(ok::ApplicationFeature::AutoClearStencil);

	time_bar = new ok::graphics::LineBatch(1);
}


void Zoner::Game::Update(float dt)
{
	if (StateFalse(Zoner::GameStates::DefaultResourcesLoaded) && StateFalse(Zoner::GameStates::PreloaderVisible))
	{
		_preloader.Task_DefaultResources();
		ShowPreloader();
	}

	if (StateTrue(Zoner::GameStates::DefaultResourcesLoaded) && StateTrue(Zoner::GameStates::PreloaderVisible))
	{
		HidePreloader();
	}

	if (StateTrue(Zoner::GameStates::PreloaderVisible))
	{
		_preloader.Update(dt);
	}
	else
	{
		if (current_game_screen == Zoner::GameScreen::Space)
		{
			UpdateGameScreen_Space(dt);
		}
	}


	//debug time line
	ok::graphics::Camera cam(ok::graphics::CameraCoordinateSystem::Screen);
	cam.SetProjectionOrtho(static_cast<float>(screen_width), static_cast<float>(screen_height), 1.f, 100.f);
	cam.BeginTransform();
	cam.SetPosition(glm::vec3(0.f, 0.f, -10.f));
	cam.EndTransform(true);

	
	time_bar->SetBrushThickness(8.0f);
	time_bar->SetBrushSoftness(0.01f);
	time_bar->SetBrushColor(ok::Color(0.f,0.5f,1.f,1.f));

	ok::graphics::Camera::PushCamera(&cam);
	time_bar->BatchBegin();
		time_bar->MoveTo(glm::vec3(0.f, 4.f, 0.f));
		time_bar->LineTo(glm::vec3(day_progress * static_cast<float>(screen_width), 4.f, 0.f));
	time_bar->BatchEnd();
	ok::graphics::Camera::PopCamera();
}

void Zoner::Game::LoadDefaultGame()
{

}

void Zoner::Game::LoadGame()
{
}

void Zoner::Game::SaveGame()
{
}

void Zoner::Game::ShowPreloader()
{
	_preloader.LoadItself();
	State(Zoner::GameStates::PreloaderVisible, true);
}

void Zoner::Game::HidePreloader()
{
	State(Zoner::GameStates::PreloaderVisible, false);
	_preloader.UnloadItself();
}

bool Zoner::Game::StateTrue(Zoner::GameStates state)
{
	return _game_states[static_cast<int>(state)] == true;
}

bool Zoner::Game::StateFalse(Zoner::GameStates state)
{
	return _game_states[static_cast<int>(state)] == false;
}

void Zoner::Game::State(Zoner::GameStates state, bool value)
{
	_game_states[static_cast<int>(state)] = value;
}

void Zoner::Game::TimeStep()
{
	day_progress = 0.f;

	hour = 0;
	day++;

	if (day > 30)
	{
		day = 1;
		month++;
	}

	if (month > 12)
	{
		month = 1;
		year++;
	}

	if (year > 100)
	{
		year = 1;
		epoch++;
	}

	if (epoch > 10)
	{
		epoch = 1;
		millennia++;
	}

	if (millennia > 1000)
	{
		millennia = 1;

		if (era == std::numeric_limits<unsigned int>::max())
		{
			era = std::numeric_limits<unsigned int>::min();
		}
		else
		{
			era++;
		}
	}
}

void Zoner::Game::UpdateGameScreen_Space(float dt)
{
	if (StateFalse(Zoner::GameStates::PauseEnabled))
	{
		float day_progress_delta = (dt / real_seconds_per_day);
		day_progress += day_progress_delta;
		hour = static_cast<unsigned int>(glm::floor(24.0f * day_progress));

		//update all spaces here
		//current space update in realtime
		if (_current_space != nullptr)
		{
			_current_space->PassTime(day_progress_delta*24.0f);
			_current_space->ApplyPassedTime();
		}

		//other spaces update 4 times per day (every 8 hours, every second in realtime)
		if (hour >= 0 && daily_updates_done == 0)
		{
			//update outer spaces
			for (auto& space : _spaces)
			{
				if (space.second != _current_space)
				{
					space.second->PassTime(8);
					space.second->ApplyPassedTime();
				}

			}
			daily_updates_done++;
		}

		if (hour >= 7 && daily_updates_done == 1)
		{
			//update outer spaces
			for (auto& space : _spaces)
			{
				if (space.second != _current_space)
				{
					space.second->PassTime(8);
					space.second->ApplyPassedTime();
				}
			}
			daily_updates_done++;
		}

		if (hour >= 15 && daily_updates_done == 2)
		{
			//update outer spaces
			for (auto& space : _spaces)
			{
				if (space.second != _current_space)
				{
					space.second->PassTime(8);
					space.second->ApplyPassedTime();
				}
			}
			daily_updates_done++;
		}

		if (hour >= 23 && daily_updates_done == 3)
		{
			//update outer spaces
			for (auto& space : _spaces)
			{
				if (space.second != _current_space)
				{
					space.second->PassTime(8);
					space.second->ApplyPassedTime();
				}
			}
			daily_updates_done++;
		}

		if (hour >= 24)
		{
			TimeStep();

			//allow everyone except player to do decisions here

			if (StateTrue(Zoner::GameStates::PauseRequest))
			{
				State(Zoner::GameStates::PauseEnabled, true);
				State(Zoner::GameStates::PauseRequest, false);
			}
			daily_updates_done = 0;
		}

		//------
		if (ok::Input::o().KeyPressed(ok::KKey::Space)) State(Zoner::GameStates::PauseRequest, true);
	}
	else
	{
		//allow player to do some decisions and press play

		if (ok::Input::o().KeyPressed(ok::KKey::Space))
		{
			State(Zoner::GameStates::PauseEnabled, false);
			State(Zoner::GameStates::PauseRequest, false);
		}
	}

	//animate and render current space here
	if (_current_space != nullptr)
	{
		_current_space->Update(dt);
	}
}

int Zoner::Game::GetScreenWidth()
{
	return screen_width;
}

int Zoner::Game::GetScreenHeight()
{
	return screen_height;
}

std::unordered_map<std::string, Zoner::ShipBlueprint*>& Zoner::Game::GetShipBlueprints()
{
	return _ship_blueprints;
}
