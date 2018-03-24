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
	State(Zoner::GameStates::AtStartup, true);
	time_bar = new ok::graphics::LineBatch(1);
}


void Zoner::Game::Update(float dt)
{
	if (StateTrue(Zoner::GameStates::AtStartup))
	{
		if (StateFalse(Zoner::GameStates::DefaultResourcesLoaded) && StateFalse(Zoner::GameStates::PreloaderVisible))
		{
			_preloader.Task_DefaultResources();
			ShowPreloader();
		}

		if (StateTrue(Zoner::GameStates::DefaultResourcesLoaded))
		{
			HidePreloader();

			LoadUserSettings();
			LoadGame();

			State(Zoner::GameStates::AtStartup, false);
		}
	}

	if (StateTrue(Zoner::GameStates::LoadGame))
	{
		if (StateFalse(Zoner::GameStates::PreloaderVisible))
		{
			_preloader.Task_ShowProgress();
			ShowPreloader();
			LoadGameBegin();
		}
		else
		{
			if (StateTrue(Zoner::GameStates::LoadGameCompleted))
			{
				LoadGameEnd();
				HidePreloader();
			}
			else
			{
				LoadGameUpdate();
			}
		}
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

void Zoner::Game::SetGameName(ok::String game_name)
{
	_current_game_name = game_name;
}

void Zoner::Game::LoadGame()
{
	ok::String folder = ok::Assets::instance().assets_root_folder + "saves\\" + _current_game_name + "\\";
	ok::String file = folder + "save." + _current_game_name + ".xml";
	std::string std_path = file;

	_game_file.Clear();
	_game_file.LoadFile(std_path.c_str());

	State(Zoner::GameStates::LoadGame, true);
	State(Zoner::GameStates::LoadGameCompleted, false);
}

void Zoner::Game::SaveGame()
{
	ok::String folder = ok::Assets::instance().assets_root_folder + "saves\\" + _current_game_name + "\\";
	ok::String file = folder + "save." + _current_game_name + ".xml";
}

void Zoner::Game::LoadUserSettings()
{
	std::string std_path = "zoner.user.settings.xml";
	tinyxml2::XMLDocument doc;
	doc.LoadFile(std_path.c_str());

	tinyxml2::XMLElement* elem;
	tinyxml2::XMLElement* inner_elem;

	elem = doc.FirstChildElement("user_settings");

	inner_elem = elem->FirstChildElement("last_game");
	SetGameName(inner_elem->Attribute("name"));
}

void Zoner::Game::SaveUserSettings()
{
	std::string std_path = "zoner.user.settings.xml";
	tinyxml2::XMLDocument doc;
	doc.LoadFile(std_path.c_str());

	tinyxml2::XMLElement* elem;
	tinyxml2::XMLElement* inner_elem;

	elem = doc.FirstChildElement("user_settings");

	inner_elem = elem->FirstChildElement("last_game");
	inner_elem->SetAttribute("name", _current_game_name.toAnsiString().c_str());

	doc.SaveFile(std_path.c_str());
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

void Zoner::Game::LoadGameBegin()
{
	_load_save_game_stage = 0;
}

void Zoner::Game::LoadGameUpdate()
{
	if (_load_save_game_stage == 0)
	{
		_game_file_element = _game_file.FirstChildElement("save")->FirstChildElement("spaces");
		_game_file_element_iterator = _game_file_element->FirstChildElement("space");

		_load_save_game_step = 0;
		_load_save_game_step_max = _game_file_element->IntAttribute("count");

		_load_save_game_stage = 1;
	}

	if (_load_save_game_stage == 1 && _load_save_game_step <= _load_save_game_step_max)
	{
		if (_load_save_game_step < _load_save_game_step_max)
		{
			Zoner::Space* space = new Zoner::Space();

			space->Rename(_game_file_element_iterator->Attribute("name"));

			_spaces[_game_file_element_iterator->Attribute("id")] = space;
			_game_file_element_iterator = _game_file_element_iterator->NextSiblingElement("space");

			_load_save_game_step++;
		}
		else
		{
			_load_save_game_stage = 2;
		}
	}

	if (_load_save_game_stage == 2)
	{
		_game_file_element = _game_file.FirstChildElement("save")->FirstChildElement("space_connections");
		_game_file_element_iterator = _game_file_element->FirstChildElement("connection");

		_load_save_game_step = 0;
		_load_save_game_step_max = _game_file_element->IntAttribute("count");

		_load_save_game_stage = 3;
	}

	if (_load_save_game_stage == 3 && _load_save_game_step <= _load_save_game_step_max)
	{
		if (_load_save_game_step < _load_save_game_step_max)
		{
			Zoner::SpaceConnection* connection = new Zoner::SpaceConnection();
			connection->destination = _spaces[_game_file_element_iterator->Attribute("to")];
			connection->position = glm::vec3(
				_game_file_element_iterator->FloatAttribute("x"),
				_game_file_element_iterator->FloatAttribute("y"),
				0.0f
			);
			connection->radius = glm::vec3(
				_game_file_element_iterator->FloatAttribute("rx"),
				_game_file_element_iterator->FloatAttribute("ry"),
				0.0f
			);

			_spaces[_game_file_element_iterator->Attribute("from")]->connections.push_back(connection);

			_load_save_game_step++;
			_game_file_element_iterator = _game_file_element_iterator->NextSiblingElement("connection");
		}
		else
		{
			_load_save_game_stage = 4;
		}
	}

	if (_load_save_game_stage == 4)
	{
		_game_file_element = _game_file.FirstChildElement("save")->FirstChildElement("spacecrafts");
		_game_file_element_iterator = _game_file_element->FirstChildElement("spacecraft");

		_load_save_game_step = 0;
		_load_save_game_step_max = _game_file_element->IntAttribute("count");

		_load_save_game_stage = 5;
	}

	if (_load_save_game_stage == 5 && _load_save_game_step <= _load_save_game_step_max)
	{
		if (_load_save_game_step < _load_save_game_step_max)
		{
			Zoner::Ship* ship = new Zoner::Ship();

			ship->this_type = Zoner::ShipType::Spacecraft;
			ship->Rename(_game_file_element_iterator->Attribute("name"));
			ship->Relocate(_spaces[_game_file_element_iterator->FirstChildElement("location")->Attribute("space_id")]);

			tinyxml2::XMLElement* position = _game_file_element_iterator->FirstChildElement("position");
			tinyxml2::XMLElement* rotation = _game_file_element_iterator->FirstChildElement("rotation");

			ship->BeginTransform();
			ship->SetPosition(glm::vec3(position->FloatAttribute("x"), position->FloatAttribute("y"), position->FloatAttribute("z")));
			ship->SetRotation(glm::vec3(rotation->FloatAttribute("x"), rotation->FloatAttribute("y"), rotation->FloatAttribute("z")));
			ship->EndTransform(true);

			ship->this_blueprint = static_cast<Zoner::ShipBlueprint*>(_ship_blueprints[_game_file_element_iterator->Attribute("blueprint")]->Duplicate());
			ship->AddChild(ship->this_blueprint);

			ship->isNPC = _game_file_element_iterator->BoolAttribute("isNPC");

			if (ship->isNPC == false)
			{
				_current_space = static_cast<Zoner::Space*>(ship->Location());
				_current_player_ship = ship;
			}

			_load_save_game_step++;
			_game_file_element_iterator = _game_file_element_iterator->NextSiblingElement("spacecraft");
		}
		else
		{
			_load_save_game_stage = 6;
		}
	}

	if (_load_save_game_stage == 6)
	{
		State(Zoner::GameStates::LoadGameCompleted, true);
	}

	_preloader.Task_ShowProgress_Update(_load_save_game_step, _load_save_game_step_max);
}

void Zoner::Game::LoadGameEnd()
{
	State(Zoner::GameStates::LoadGame, false);
	State(Zoner::GameStates::LoadGameCompleted, false);
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

			//Reset some variables daily and etc
			for (auto& space : _spaces)
			{
				space.second->OnNewDay();
			}

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
		if (_current_player_ship != nullptr)
		{
			_current_player_ship->Player_UpdateDecisions(dt);
		}


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
