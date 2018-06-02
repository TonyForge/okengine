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

	if (StateTrue(Zoner::GameStates::SaveGame))
	{
		if (StateFalse(Zoner::GameStates::PreloaderVisible))
		{
			_preloader.Task_ShowProgress();
			ShowPreloader();
			SaveGameBegin();
		}
		else
		{
			if (StateTrue(Zoner::GameStates::SaveGameCompleted))
			{
				SaveGameEnd();
				HidePreloader();
			}
			else
			{
				SaveGameUpdate();
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

	ok::String folder = ok::Assets::instance().assets_root_folder + "saves\\" + _current_game_name;

	std::experimental::filesystem::create_directories(folder.toAnsiString());
	//std::experimental::filesystem::create_directories((folder + "\\per_object_data").toAnsiString());
	
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
	_game_file.Clear();

	State(Zoner::GameStates::SaveGame, true);
	State(Zoner::GameStates::SaveGameCompleted, false);
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

void Zoner::Game::SaveTo(tinyxml2::XMLDocument & doc, tinyxml2::XMLElement & element)
{
}

void Zoner::Game::LoadFrom(tinyxml2::XMLDocument & doc, tinyxml2::XMLElement & element)
{
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
			space->_gameengine_id = _game_file_element_iterator->Attribute("id");

			_spaces[space->_gameengine_id] = space;

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
		_game_file_element = _game_file.FirstChildElement("save")->FirstChildElement("objects");
		_game_file_element_iterator = _game_file_element->FirstChildElement("space");

		_load_save_game_step = 0;
		_load_save_game_step_max = _game_file_element->IntAttribute("count");

		_load_save_game_stage = 3;
		_load_save_game_substage = 0;
	}

	if (_load_save_game_stage == 3 && _load_save_game_step <= _load_save_game_step_max)
	{
		if (_load_save_game_step < _load_save_game_step_max)
		{
			if (_load_save_game_substage == 0)
			{
				_game_file_element_2 = _game_file_element_iterator->FirstChildElement("space_jump_holes");
				if (_game_file_element_2->IntAttribute("count") == 0)
				{
					_load_save_game_substage = 2;
				}
				else
				{
					_game_file_element_2 = _game_file_element_2->FirstChildElement("jump_hole");
					_load_save_game_substage = 1;
				}
				
			}

			if (_load_save_game_substage == 1)
			{
				Zoner::JumpHole* _jump_hole = new Zoner::JumpHole();

				_jump_hole->location = _spaces[_game_file_element_2->Attribute("from")];
				_jump_hole->destination = _spaces[_game_file_element_2->Attribute("to")];

				_jump_hole->LoadFrom(_game_file, *_game_file_element_2);

				_spaces[_game_file_element_iterator->Attribute("id")]->jump_holes.push_back(_jump_hole);

				_game_file_element_2 = _game_file_element_2->NextSiblingElement();

				if (_game_file_element_2 == nullptr)
				{
					_load_save_game_substage = 2;
				}

				_load_save_game_step++;
			}

			if (_load_save_game_substage == 2)
			{
				_game_file_element_2 = _game_file_element_iterator->FirstChildElement("visitors");
				if (_game_file_element_2->IntAttribute("count") == 0)
				{
					_load_save_game_substage = 4;
				}
				else
				{
					_game_file_element_2 = _game_file_element_2->FirstChildElement("visitor");
					_load_save_game_substage = 3;
				}
			}

			if (_load_save_game_substage == 3)
			{
				Zoner::Ship* ship = new Zoner::Ship();

				ship->LoadFrom(_game_file, *_game_file_element_2);

				if (ship->isNPC == false)
				{
					_current_player_ship = ship;
				}

				ship->Relocate(_spaces[_game_file_element_2->FirstChildElement("location")->Attribute("space_id")]);

				tinyxml2::XMLElement* position = _game_file_element_2->FirstChildElement("position");
				tinyxml2::XMLElement* rotation = _game_file_element_2->FirstChildElement("rotation");

				ship->BeginTransform();
				ship->SetPosition(glm::vec3(position->FloatAttribute("x"), position->FloatAttribute("y"), position->FloatAttribute("z")));
				ship->SetRotation(glm::vec3(rotation->FloatAttribute("x"), rotation->FloatAttribute("y"), rotation->FloatAttribute("z")));
				ship->EndTransform(true);

				//Potom sdelat chtobi vmesto etogo sohranalos pologenie cameri v xml i ottuda zagrugalos
				if (ship->isNPC == false)
				{
					glm::vec3& cam_pos = ship->GetPosition();
					cam_pos.z = ship->location->camera.GetPosition().z;
					ship->location->camera.SetPosition(cam_pos);
				}

				_game_file_element_2 = _game_file_element_2->NextSiblingElement();

				if (_game_file_element_2 == nullptr)
				{
					_load_save_game_substage = 4;
				}

				_load_save_game_step++;
			}


			if (_load_save_game_substage == 4)
			{
				_load_save_game_substage = 0;
				_game_file_element_iterator = _game_file_element_iterator->NextSiblingElement("space");
			}
		}
		else
		{
			_load_save_game_stage = 4;
		}
	}

	if (_load_save_game_stage == 4)
	{
		//must be here! Objecti kogda zagruzhayutsa potreblyat uid, mi hotim etogo izbeshat, poetomu forceim nugniy nam global uid v samom konce
		Zoner::UID::load_global_uid_from_xml(*(_game_file.FirstChildElement("save")->FirstChildElement("game_data")->FirstChildElement("uid")));

		State(Zoner::GameStates::LoadGameCompleted, true);

		State(Zoner::GameStates::PauseRequest, true);
		State(Zoner::GameStates::PauseEnabled, true);
	}

	_preloader.Task_ShowProgress_Update(_load_save_game_step, _load_save_game_step_max);
}

void Zoner::Game::LoadGameEnd()
{
	State(Zoner::GameStates::LoadGame, false);
	State(Zoner::GameStates::LoadGameCompleted, false);

	_game_file.Clear();
}

void Zoner::Game::SaveGameBegin()
{
	_load_save_game_stage = 0;
}

void Zoner::Game::SaveGameUpdate()
{
	if (_load_save_game_stage == 0)
	{
		_load_save_game_step = 0;
		_load_save_game_step_max = _spaces.size();

		_game_file_root_element = _game_file.NewElement("save");
		_game_file.InsertFirstChild(_game_file_root_element);

		_game_file_root_element->InsertEndChild(_game_file.NewElement("game_data"));
		_game_file_root_element->FirstChildElement("game_data")->InsertEndChild(_game_file.NewElement("uid"));
		Zoner::UID::save_global_uid_to_xml(*(_game_file_root_element->FirstChildElement("game_data")->FirstChildElement("uid")));

		_game_file_element = _game_file.NewElement("spaces");
		_game_file_root_element->InsertEndChild(_game_file_element);

		_game_file_element_2 = _game_file.NewElement("objects");
		_game_file_root_element->InsertEndChild(_game_file_element_2);

		_game_file_element_2->SetAttribute("count", static_cast<unsigned int>(0));

		_game_file_element->SetAttribute("count", _load_save_game_step_max);

		_spaces_iterator = _spaces.begin();

		_load_save_game_stage = 1;
	}

	if (_load_save_game_stage == 1 && _load_save_game_step <= _load_save_game_step_max)
	{
		if (_load_save_game_step < _load_save_game_step_max)
		{
			_game_file_element_iterator = _game_file.NewElement("space");
			_game_file_element->InsertEndChild(_game_file_element_iterator);

			_game_file_element_iterator->SetAttribute("id", _spaces_iterator->second->_gameengine_id.toAnsiString().c_str());
			_game_file_element_iterator->SetAttribute("name", _spaces_iterator->second->name.toAnsiString().c_str());

			_game_file_element_2->SetAttribute("count", _game_file_element_2->UnsignedAttribute("count") + 
				static_cast<unsigned int>(_spaces_iterator->second->jump_holes.size()) + 
				static_cast<unsigned int>(_spaces_iterator->second->visitors.size())
			);

			_spaces_iterator++;

			_load_save_game_step++;
		}
		else
		{
			_load_save_game_stage = 2;
			_load_save_game_substage = -1;

			_spaces_iterator = _spaces.begin();
		}	
	}

	if (_load_save_game_stage == 2)
	{
		if (_load_save_game_substage == -1)
		{
			_game_file_element_3 = _game_file.NewElement("space");
			_game_file_element_3->SetAttribute("id", _spaces_iterator->second->_gameengine_id.toAnsiString().c_str());

			_game_file_element_2->InsertEndChild(_game_file_element_3);

			_load_save_game_substage = 0;
		}

		//jump holes
		if (_load_save_game_substage == 0)
		{
			_load_save_game_step = 0;
			_load_save_game_step_max = _spaces_iterator->second->jump_holes.size();

			_game_file_element_iterator_2 = _game_file.NewElement("space_jump_holes");
			_game_file_element_iterator_2->SetAttribute("count", _load_save_game_step_max);

			_game_file_element_3->InsertEndChild(_game_file_element_iterator_2);

			_load_save_game_substage = 1;
		}

		if (_load_save_game_substage == 1 && _load_save_game_step <= _load_save_game_step_max)
		{
			if (_load_save_game_step < _load_save_game_step_max)
			{
				auto _element = _game_file.NewElement("jump_hole");
				auto _object = _spaces_iterator->second->jump_holes[_load_save_game_step];

				_object->SaveTo(_game_file, *_element);

				_game_file_element_iterator_2->InsertEndChild(_element);

				_load_save_game_step++;
			}
			else
			{
				_load_save_game_substage = 2;
			}
		}

		//visitors
		if (_load_save_game_substage == 2)
		{
			_load_save_game_step = 0;
			_load_save_game_step_max = _spaces_iterator->second->visitors.size();

			_game_file_element_iterator_2 = _game_file.NewElement("visitors");
			_game_file_element_iterator_2->SetAttribute("count", _load_save_game_step_max);

			_game_file_element_3->InsertEndChild(_game_file_element_iterator_2);

			_load_save_game_substage = 3;
		}

		if (_load_save_game_substage == 3 && _load_save_game_step <= _load_save_game_step_max)
		{
			if (_load_save_game_step < _load_save_game_step_max)
			{
				auto _element = _game_file.NewElement("visitor");
				auto _object = _spaces_iterator->second->visitors[_load_save_game_step];

				_object->SaveTo(_game_file, *_element);

				_game_file_element_iterator_2->InsertEndChild(_element);

				_load_save_game_step++;
			}
			else
			{
				_load_save_game_substage = 4;
			}
		}

		if (_load_save_game_substage == 4)
		{
			_spaces_iterator++;

			if (_spaces_iterator == _spaces.end())
			{
				_load_save_game_stage = 3;
			}
			else
			{
				_load_save_game_substage = -1;
			}
		}
	}


	if (_load_save_game_stage == 3)
	{
		State(Zoner::GameStates::SaveGameCompleted, true);
	}

	_preloader.Task_ShowProgress_Update(_load_save_game_step, _load_save_game_step_max);
}

void Zoner::Game::SaveGameEnd()
{
	State(Zoner::GameStates::SaveGame, false);
	State(Zoner::GameStates::SaveGameCompleted, false);

	ok::String folder = ok::Assets::instance().assets_root_folder + "saves\\" + _current_game_name + "\\";
	ok::String file = folder + "save." + _current_game_name + ".xml";
	std::string std_path = file;

	_game_file.SaveFile(std_path.c_str());
	_game_file.Clear();
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

Zoner::IShip * Zoner::Game::GetCurrentPlayerShip()
{
	return _current_player_ship;
}

void Zoner::Game::ChangeCurrentLocation(Zoner::ISpace * location)
{
	if (_current_space != nullptr)
	{
		_current_space->isCurrent = false;
		_current_space = nullptr;
	}

	_current_space = static_cast<Zoner::Space*>(location);
	_current_space->isCurrent = true;
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
	Zoner::CommandsList::AllListsExecute();

	if (StateFalse(Zoner::GameStates::PauseEnabled))
	{
		float day_progress_delta = (dt / real_seconds_per_day);
		day_progress += day_progress_delta;
		hour = static_cast<unsigned int>(glm::floor(24.0f * day_progress));

		/*if (_current_player_ship != nullptr)
		{
			if (_current_space != nullptr)
			{
				_current_space->isCurrent = false;
				_current_space = nullptr;
			}

			_current_space = static_cast<Zoner::Space*>(_current_player_ship->Location());
			_current_space->isCurrent = true;
		}*/

		//update all spaces here
		Zoner::CommandsList::AllListsPassTime(day_progress_delta*24.0f);
		Zoner::CommandsList::AllListsApplyPassedTime();

		if (hour >= 24)
		{
			TimeStep();

			State(Zoner::GameStates::CancelAutoPause, false);

			//Reset some variables daily and etc, and allow everyone except player to do decisions here
			Zoner::CommandsList::AllListsOnNewDay();

			if (StateTrue(Zoner::GameStates::CancelAutoPause))
			{
				State(Zoner::GameStates::PauseEnabled, false);	
			}
			else
			{
				State(Zoner::GameStates::PauseEnabled, true);
			}

			if (StateTrue(Zoner::GameStates::PauseRequest))
			{
				State(Zoner::GameStates::PauseEnabled, true);
				//State(Zoner::GameStates::PauseRequest, false);
			}
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

			if (_current_space != nullptr)
			{
				_current_space->DoCameraFollow();
			}
		}
		else
		{
			if (ok::Input::o().KeyDown(ok::KKey::LControl) && ok::Input::o().KeyPressed(ok::KKey::S))
			{
				SaveGame();
			}
		}
	}

	//run animation commands
	Zoner::CommandsList::AllListsUpdate(dt);

	//animate and render current space here
	if (_current_space != nullptr)
	{
		_current_space->Update(dt);
	}

	Zoner::SpaceScreenGUI::Update(dt);
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

std::unordered_map<std::string, Zoner::ItemBlueprint*>& Zoner::Game::GetItemBlueprints()
{
	return _item_blueprints;
}

std::unordered_map<std::string, ok::graphics::SpriteAtlas*>& Zoner::Game::GetSpriteAtlases()
{
	return _sprite_atlases;
}
