#include "DOEGame.h"

DOE::IDOEGame* DOE::IDOEGame::_instance = nullptr;

DOE::DOEGame::DOEGame()
{
	_settings_file_name = "denofevil.settings.xml";
	_instance = this;
}

void DOE::DOEGame::Init()
{
	DisableFeature(ok::ApplicationFeature::AutoClearStencil);

	def_batch = new ok::graphics::SpriteBatch();
	scene_root = new ok::GameObject();

	def_text_batch = new ok::graphics::TextBatch2D(DOE::global.g_screen_width, DOE::global.g_screen_height);
	
	//ConvertTMXToChunk(ok::String("denofevil_assets\\maps\\map1.tmx"), ok::String("denofevil_assets\\game_slots\\slot01"));
	LoadGameFromSlot(1);
}

void DOE::DOEGame::Update(float dt)
{
	global.g_camera.BeginTransform();
	global.g_camera.SetPosition(global.g_camera.GetPosition() + (glm::vec3(_camera_target_x,_camera_target_y,global.g_camera.GetPosition().z) - global.g_camera.GetPosition())*_camera_speed);
	global.g_camera.EndTransform(true);

	if (ok::Input::o().KeyDown(ok::KKey::Left))
	{
		SetCameraTarget(_camera_target_x - 1.f, _camera_target_y);
	}
	if (ok::Input::o().KeyDown(ok::KKey::Right))
	{
		SetCameraTarget(_camera_target_x + 1.f, _camera_target_y);
	}
	if (ok::Input::o().KeyDown(ok::KKey::Up))
	{
		SetCameraTarget(_camera_target_x, _camera_target_y - 1.f);
	}
	if (ok::Input::o().KeyDown(ok::KKey::Down))
	{
		SetCameraTarget(_camera_target_x, _camera_target_y + 1.f);
	}

	scene_root->Update(dt);

	//floor1.Render(*def_batch, GetCameraViewport());
	floors.back().UpdateFOV(_camera_target_x, _camera_target_y, GetCameraViewport());

	floors.back().Render(*def_batch, GetCameraViewport());
	
}

void DOE::DOEGame::ConvertTMXToChunk(ok::String & in, ok::String & out)
{
	std::vector<std::pair<int,DOE::DOEFloor*>> floors;
	auto GetFloor = [&floors](int floor_id) -> DOE::DOEFloor*
	{
		for (auto& floor_pair : floors)
		{
			if (floor_pair.first == floor_id)
			{
				return floor_pair.second;
			}
		}

		DOE::DOEFloor* fl = new DOE::DOEFloor();
		floors.push_back(std::make_pair(floor_id, fl));
		return fl;
	};

	tinyxml2::XMLDocument doc;
	doc.LoadFile(in.toAnsiString().c_str());

	tinyxml2::XMLElement* elem;
	tinyxml2::XMLElement* inner_elem_layers;

	elem = doc.FirstChildElement("map");

	//inner_elem_layers = elem->FirstChildElement("layer");

	ok::String str_attrib;

	for (tinyxml2::XMLElement* child = elem->FirstChildElement("layer"); child != NULL; child = child->NextSiblingElement("layer"))
	{
		int floor_id;
		bool is_overlay;

		tinyxml2::XMLElement* properties = child->FirstChildElement("properties");
		for (tinyxml2::XMLElement* property_elem = properties->FirstChildElement("property"); property_elem != NULL; property_elem = property_elem->NextSiblingElement("property"))
		{
			str_attrib = property_elem->Attribute("name");

			if (str_attrib == "floor")
			{
				floor_id = property_elem->IntAttribute("value");
			}
			else
			if (str_attrib == "overlay")
			{
				is_overlay = property_elem->BoolAttribute("value");
			}
		}

		DOE::DOEFloor* pfloor = GetFloor(floor_id);
		tinyxml2::XMLElement* tiles_data = child->FirstChildElement("data");

		if (tiles_data)
		{
			auto tiles_text_data = tiles_data->GetText();
			int i = 0;
			str_attrib = "";

			int tile_id = 0;
			int tile_x = 0;
			int tile_y = 0;

			while (true)
			{

				if ((str_attrib.isEmpty() == false) &&
					(tiles_text_data[i] == 10 ||
					tiles_text_data[i] == 13 ||
					tiles_text_data[i] == ','))
				{
					//process buffer and clean it
					tile_id = std::stoi(str_attrib.toAnsiString().c_str());
					str_attrib.clear();

					//if (tile_id == 0) tile_id = -1;
					tile_id--; //because empty tile must be -1

					//place tile_id at tile_x tile_y
					DOE::FloorTile& ftile = pfloor->GetTile(tile_x, tile_y);
					if (is_overlay)
					{
						if (tile_id != -1)
						ftile.overlays.push_back(tile_id);
					}
					else
					{
						ftile.sprite_id = tile_id;

						switch (tile_id)
						{
						case 0: ftile.solid = true; break;
						case 1: ftile.solid = false; break;
						default: ftile.solid = false;
						}
					}

					

					//if (tile_id == 0)
						//ftile.solid = true;

					tile_x++;
					if (tile_x == DOE::global.g_floor_size)
					{
						tile_x = 0;
						tile_y++;
						if (tile_y == DOE::global.g_floor_size)
						{
							break;
						}
					}

					
				}
				else
				{
					str_attrib += tiles_text_data[i];
				}
				i++;
			}
		}
		
	}

	auto sort_floors_fnc = [](std::pair<int, DOE::DOEFloor*> left, std::pair<int, DOE::DOEFloor*> right)->bool
	{
		return (left.first < right.first);
	};
	std::sort(floors.begin(), floors.end(), sort_floors_fnc);

	int grid_tile_template[5];

	std::ofstream fo;
	fo.open((const std::string&)(out+"_grid_data.chunk"), std::ofstream::out | std::ofstream::binary);
	
	for (auto& floor : floors)
	{
		for (int y = 0; y < DOE::global.g_floor_size; y++)
		for (int x = 0; x < DOE::global.g_floor_size; x++)
		{
			auto tile = floor.second->GetTile(x, y);
			grid_tile_template[0] = tile.sprite_id;
			grid_tile_template[1] = -1; grid_tile_template[2] = -1; grid_tile_template[3] = -1;
			if (tile.overlays.size() > 0) grid_tile_template[1] = tile.overlays[0];
			if (tile.overlays.size() > 1) grid_tile_template[2] = tile.overlays[1];
			if (tile.overlays.size() > 2) grid_tile_template[3] = tile.overlays[2];
			grid_tile_template[4] = 0;
			if (tile.solid) 
				grid_tile_template[4] = (grid_tile_template[4] | 1);

			fo.write(reinterpret_cast<char*>(grid_tile_template), sizeof(grid_tile_template));
		}
		
	}

	fo.close();
	//inner_elem_layers = elem->FirstChildElement("objectgroup");

	for (tinyxml2::XMLElement* child = elem->FirstChildElement("objectgroup"); child != NULL; child = child->NextSiblingElement("objectgroup"))
	{

	}
}

void DOE::DOEGame::LoadGameFromSlot(int slot_id)
{
	ok::String path = "denofevil_assets\\game_slots\\slot0";
	path += std::to_string(slot_id);

	int grid_tile_template[5];

	tinyxml2::XMLDocument run_doc;
	run_doc.LoadFile(ok::String(path + "\\run.xml").toAnsiString().c_str());

	tinyxml2::XMLElement* elem;
	tinyxml2::XMLElement* inner_elem_layers;

	elem = run_doc.FirstChildElement("run")->FirstChildElement("grid");
	ok::String grid_data = path + "\\" +elem->Attribute("file");

	int grid_data_counter = 0;
	int grid_x = 0;
	int grid_y = 0;
	std::ifstream fi;

	fi.open(grid_data.toAnsiString().c_str(), std::ifstream::in | std::ifstream::binary);
	while (true)
	{
		fi.read(reinterpret_cast<char *>(grid_tile_template), sizeof(grid_tile_template));
		if (fi.eof()) break;

		if (grid_data_counter == 0)
		{
			floors.resize(floors.size() + 1);
		}

		auto& tile = floors.back().GetTile(grid_x, grid_y);
		tile.overlays.clear();
		tile.sprite_id = grid_tile_template[0];
		tile.overlays.push_back(grid_tile_template[1]);
		tile.overlays.push_back(grid_tile_template[2]);
		tile.overlays.push_back(grid_tile_template[3]);
		tile.solid = (grid_tile_template[4] & 1);

		grid_data_counter++;
		grid_x++;
		if (grid_x == DOE::global.g_floor_size)
		{
			grid_x = 0;
			grid_y++;

			if (grid_y == DOE::global.g_floor_size)
			{
				grid_x = 0;
				grid_y = 0;
				grid_data_counter = 0;
			}
		}
	}
	fi.close();
	//std::ifstream fi;
	//fi.open(path+"\\run.xml")
}

void DOE::DOEGame::PlaceCamera(float x, float y)
{
	global.g_camera.BeginTransform();
	global.g_camera.SetPosition(glm::vec3(x, y, global.g_camera.GetPosition().z));
	global.g_camera.EndTransform(true);
}

void DOE::DOEGame::SetCameraSpeed(float speed)
{
	_camera_speed = speed;
}

void DOE::DOEGame::SetCameraTarget(float x, float y)
{
	_camera_target_x = x;
	_camera_target_y = y;
}

ok::Rect2Df DOE::DOEGame::GetCameraViewport()
{
	return global.g_camera.GetOrthoViewportXY();
}

ok::graphics::TextBatch2D * DOE::DOEGame::GetTextBatch()
{
	return def_text_batch;
}
