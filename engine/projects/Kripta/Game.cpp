#include "Game.h"

Kripta::IGame* Kripta::IGame::instance = nullptr;

Kripta::Game::Game()
{
	_settings_file_name = "kripta.settings.xml";
	instance = this;
}

void Kripta::Game::Init()
{
	DisableFeature(ok::ApplicationFeature::AutoClearStencil);

	camera = new ok::graphics::Camera(ok::graphics::CameraCoordinateSystem::Screen);
	camera->SetProjectionOrtho(1024, 768, 100.f, 1.f);

	pp_camera = new ok::graphics::Camera(ok::graphics::CameraCoordinateSystem::Screen);
	pp_camera->SetProjectionOrtho(1024, 768, 100.f, 1.f);

	camera->BeginTransform();
	camera->SetPosition(glm::vec3(0.f, 0.f, 50.0f));
	camera->EndTransform(true);

	sprite_batch = new ok::graphics::SpriteBatch();
	line_batch = new ok::graphics::LineBatch();
	text_batch = new ok::graphics::TextBatch2D(1024, 768);

	font_def = ok::AssetsBasic::instance().GetFont("def_font");
	font_def->SetBrushOuterGlow(ok::Color::Black, 0.9f, 0.35f);
	font_def->SetBrushBold(0.25f);
	font_def->SetBrushColor(ok::Color::White);
	font_def->SetInternalFont(ok::AssetsBasic::instance().GetInternalFont("consola.font.xml"));
	text_batch->SetBrushFont(font_def);

	fov_map = new ok::FOVMap(100, 100);

	LoadRoom("map1.tmx");
}

void Kripta::Game::Update(float dt)
{
	if (dt > 1.f) dt = 1.f;

	ok::graphics::Camera::PushCamera(camera);

	ok::graphics::Texture* tex = ok::Assets::instance().GetTexture("tiles.png");
	ok::graphics::SpriteInfo info;
	//info.

	glm::vec3 camera_pos;

	camera->BeginTransform(ok::TransformSpace::WorldSpace);
	camera_pos = camera->GetPosition();
	camera->EndTransform(false);

	if (camera_pos.x < -1023) camera_pos.x = -1023;
	if (camera_pos.x > 32*100+1023) camera_pos.x = 32 * 100 + 1023;

	if (camera_pos.y < -767) camera_pos.y = -767;
	if (camera_pos.y > 32 * 100 + 767) camera_pos.y = 32 * 100 + 767;


	float delta = glm::length(glm::vec2(room.camera_x, room.camera_y) - glm::vec2(camera_pos.x, camera_pos.y));

	if (delta <= 1.f)
	{
		camera->BeginTransform(ok::TransformSpace::WorldSpace);
		camera->SetPosition(glm::vec3(room.camera_x, room.camera_y, 0.f));
		camera->EndTransform(false);
	}
	else
	{
		camera->BeginTransform(ok::TransformSpace::WorldSpace);
		camera->SetPosition(camera_pos + glm::vec3(room.camera_x - camera_pos.x, room.camera_y - camera_pos.y, 0.f) * dt * 4.f);
		camera->EndTransform(false);
	}


	camera->BeginTransform(ok::TransformSpace::WorldSpace);
	camera_pos = camera->GetPosition();
	camera->EndTransform(false);



	pp_camera->BeginTransform(ok::TransformSpace::WorldSpace);
	pp_camera->SetPosition(glm::floor(camera_pos));
	pp_camera->EndTransform(false);

	ok::Rect2Df camera_rect;
	ok::Rect2Df room_rect;

	camera_rect.SetLTRB(camera_pos.x, camera_pos.y, camera_pos.x + 1024.f, camera_pos.y + 768.f);
	room_rect.SetLTRB(0.f, 0.f, 100.f*32.f, 100.f*32.f);
	ok::Rect2Df overlap_rect = room_rect.GetOverlap(camera_rect);


	int tiles_rect_left = static_cast<int>(glm::floor(overlap_rect.GetLeft() / 32.f))-1;
	int tiles_rect_top = static_cast<int>(glm::floor(overlap_rect.GetTop() / 32.f))-1;
	int tiles_rect_right = static_cast<int>(glm::floor(overlap_rect.GetRight() / 32.f)) + 1;
	int tiles_rect_bottom = static_cast<int>(glm::floor(overlap_rect.GetBottom() / 32.f)) + 1;

	if (tiles_rect_left < 0) tiles_rect_left = 0;
	if (tiles_rect_top < 0) tiles_rect_top = 0;
	if (tiles_rect_right > 99) tiles_rect_right = 99;
	if (tiles_rect_bottom > 99) tiles_rect_bottom = 99;

	int tiles_rect_width = tiles_rect_right - tiles_rect_left;//static_cast<int>(glm::ceil(overlap_rect.GetWidth() / 32.f))+2;
	int tiles_rect_height = tiles_rect_bottom - tiles_rect_top;//static_cast<int>(glm::ceil(overlap_rect.GetHeight() / 32.f))+2;

	
	//if (tiles_rect_width > 99) tiles_rect_width = 99;
	//if (tiles_rect_height > 99) tiles_rect_height = 99;


	ok::graphics::SpriteInfo spr_info;
	spr_info.rect = ok::graphics::TextureRect(tex, 0, 0, 32, 32);
	spr_info.hotspot = glm::vec2(0.f, 0.f);


	//render tiles layer 0
	ok::graphics::Camera::PushCamera(pp_camera);
	sprite_batch->BatchBegin(40.f);
	{
		int tile_id;
		int tile_x, tile_y;

		for (int ty = tiles_rect_top; ty < tiles_rect_top + tiles_rect_height; ty++)
		{
			for (int tx = tiles_rect_left; tx < tiles_rect_left + tiles_rect_width; tx++)
			{
				tile_id = room.tiles_layer_0[tx + ty * 100]-1;

				if (tile_id >= 0)
				{
					tile_x = tile_id % (512 / 32);
					tile_y = tile_id / (512 / 32);

					spr_info.rect = ok::graphics::TextureRect(tex, tile_x * 32, tile_y * 32, 32, 32);

					sprite_batch->Blit(&spr_info, tx * 32, ty * 32);
				}
			}
		}
	}
	sprite_batch->BatchEnd();
	ok::graphics::Camera::PopCamera();


	//render tiles layer 1
	ok::graphics::Camera::PushCamera(pp_camera);
	sprite_batch->BatchBegin(30.f);
	{
		int tile_id;
		int tile_x, tile_y;

		for (int ty = tiles_rect_top; ty < tiles_rect_top + tiles_rect_height; ty++)
		{
			for (int tx = tiles_rect_left; tx < tiles_rect_left + tiles_rect_width; tx++)
			{
				tile_id = room.tiles_layer_1[tx + ty * 100] - 1;

				if (tile_id >= 0)
				{
					tile_x = tile_id % (512 / 32);
					tile_y = tile_id / (512 / 32);

					spr_info.rect = ok::graphics::TextureRect(tex, tile_x * 32, tile_y * 32, 32, 32);

					sprite_batch->Blit(&spr_info, tx * 32, ty * 32);
				}
			}
		}
	}
	sprite_batch->BatchEnd();
	ok::graphics::Camera::PopCamera();

	sprite_batch->BatchBegin(25.f);
	room.Update(dt);
	sprite_batch->BatchEnd();

	//render tiles layer 2
	ok::graphics::Camera::PushCamera(pp_camera);
	sprite_batch->BatchBegin(20.f);
	{
		int tile_id;
		int tile_x, tile_y;

		for (int ty = tiles_rect_top; ty < tiles_rect_top + tiles_rect_height; ty++)
		{
			for (int tx = tiles_rect_left; tx < tiles_rect_left + tiles_rect_width; tx++)
			{
				tile_id = room.tiles_layer_2[tx + ty * 100] - 1;

				if (tile_id >= 0)
				{
					tile_x = tile_id % (512 / 32);
					tile_y = tile_id / (512 / 32);

					spr_info.rect = ok::graphics::TextureRect(tex, tile_x * 32, tile_y * 32, 32, 32);

					sprite_batch->Blit(&spr_info, tx * 32, ty * 32);
				}
			}
		}
	}
	sprite_batch->BatchEnd();
	ok::graphics::Camera::PopCamera();

	//render fov
	ok::graphics::Camera::PushCamera(pp_camera);
	sprite_batch->BatchBegin(10.f);
	{
		int fov_state;

		for (int ty = tiles_rect_top; ty < tiles_rect_top + tiles_rect_height; ty++)
		{
			for (int tx = tiles_rect_left; tx < tiles_rect_left + tiles_rect_width; tx++)
			{
				fov_state = fov_map->GetFOV(tx,ty);

				if (fov_state == false)
				{
					spr_info.rect = ok::graphics::TextureRect(tex, 2 * 32, 0 * 32, 32, 32);
					
					sprite_batch->Blit(&spr_info, tx * 32, ty * 32);
				}
			}
		}
	}
	sprite_batch->BatchEnd();
	ok::graphics::Camera::PopCamera();

	text_batch->SetBrushFont(font_def);
	text_batch->BatchBegin();
	sprite_batch->BatchBegin(5.f);
	
	for (auto obj : _post_update_list)
	{
		obj->PostUpdate(dt);
	}

	sprite_batch->BatchEnd();
	text_batch->BatchEnd();
	/*line_batch->BatchBegin();
	line_batch->LineAB(glm::vec3(0.f, 0.f, 0.f), glm::vec3(100.f, 100.f, 0.f));
	line_batch->BatchEnd();*/

	ok::graphics::Camera::PopCamera();

	//turn state processing
	if (_turn_stage == 0)
	{
		if (Kripta::TurnController::turn_members_decision_made == 0)
		{
			_turn_stage = 1;
			Kripta::TurnController::turn_members_ready = 0;
		}
	}

	if (_turn_stage == 1)
	{
		if (Kripta::TurnController::turn_members_decision_made == Kripta::TurnController::turn_members_total)
		{
			_turn_stage = 2;
			Kripta::TurnController::turn_in_progress = true;
		}
	}

	if (_turn_stage == 2)
	{
		if (Kripta::TurnController::turn_members_ready == Kripta::TurnController::turn_members_total)
		{
			_turn_stage = 3;
		}
	}

	if (_turn_stage == 3 && (Kripta::TurnController::turn_members_decision_made == 0))
	{
		_turn_stage = 4;	
	}

	if (_turn_stage == 4 && Kripta::TurnController::turn_members_died == 0)
	{
		Kripta::TurnController::turn_members_ready = 0;
		Kripta::TurnController::turn_in_progress = false;
		_turn_stage = 0;
		fov_map->CalculateFOV(static_cast<int>(glm::floor(room.hero_x / 32.f)), static_cast<int>(glm::floor(room.hero_y / 32.f)), 12, true);
		turn_number++;
	}

	
	for (auto obj : _death_list)
	{
		obj->SetParent(nullptr);
		delete obj;
	}

	_post_update_list.clear();
	_death_list.clear();
}

ok::graphics::RenderTarget * Kripta::Game::GetScreenBuffer()
{
	return fixed_resolution_framebuffer;
}

void Kripta::Game::LoadRoom(ok::String path)
{
	room.Reset();
	fov_map->Clear();

	tinyxml2::XMLDocument doc;
	doc.LoadFile((ok::String("kripta_assets/maps/")+ path).toAnsiString().c_str());

	tinyxml2::XMLElement* elem;
	tinyxml2::XMLElement* data_elem;
	tinyxml2::XMLElement* prop_elem;

	elem = doc.FirstChildElement("map");

	ok::String tile_id_str;
	int layer_type = 0;
	int layers_counter = 0;

	for (tinyxml2::XMLElement* e = elem->FirstChildElement("layer"); e != nullptr; e = e->NextSiblingElement("layer"))
	{
		data_elem = e->FirstChildElement("data");
		prop_elem = e->FirstChildElement("properties");
		auto data = &(data_elem->GetText()[1]);

		layer_type = 0;

		for (tinyxml2::XMLElement* prop = prop_elem->FirstChildElement("property"); prop != nullptr; prop = prop->NextSiblingElement("property"))
		{
			ok::String prop_name = prop->Attribute("name");

			if (prop_name == "type")
			{
				layer_type = prop->IntAttribute("value");
				layers_counter++;
			}
		}
		
		for (int y = 0; y < 100; y++)
		{
			int x = 0;

			while (*data != '\n')
			{
				tile_id_str = "";
				while ((*data != ',') && (*data != '\n'))
				{
					tile_id_str += *data;
					data++;
				}
				data++;

				int tile_id = std::stoi(tile_id_str.toAnsiString());

				if (layer_type == 1)
				{
					//tiles_layer
					if (layers_counter == 1)
					{
						room.tiles_layer_0[x + y * 100] = tile_id;

						if (tile_id == 1 /*|| tile_id == ?*/)
						{
							fov_map->SetWalkable(x, y, false);
							fov_map->SetTransparent(x, y, false);
						}
						else
						{
							fov_map->SetWalkable(x, y, true);
							fov_map->SetTransparent(x, y, true);
						}
					}
					if (layers_counter == 2)
					{
						room.tiles_layer_1[x + y * 100] = tile_id;
					}
					if (layers_counter == 3)
					{
						room.tiles_layer_2[x + y * 100] = tile_id;
					}
				}

				
				/*if (layer_type == 2)
				{
					//objects layer
					switch (tile_id)
					{
						case 129 : //hero
						{
							auto hero = new Kripta::Hero();
							hero->Place(x, y);
							room.AddChild(hero);
							SetHeroXY(static_cast<float>(x) * 32.f + 16.f, static_cast<float>(y) * 32.f + 16.f);
							hero->SetLevel(2);
						}
						break;
						case 130: //goblin
						{
							auto goblin = new Kripta::Goblin();
							goblin->Place(x, y);
							room.AddChild(goblin);
							goblin->SetLevel(1);
						}
						break;
						case 145: //gold pile
						{
							auto gold_pile = new Kripta::GoldPile();
							gold_pile->Place(x, y);
							room.AddChild(gold_pile);
						}
						break;
					}
					
				}*/

				x++;
				if (x == 99 && y == 99) break;
			}

			if (x == 99 && y == 99) break;

			data++;
		}
		

		/*for (auto sym : data)
		{

		}*/
	}

	for (tinyxml2::XMLElement* e = elem->FirstChildElement("objectgroup"); e != nullptr; e = e->NextSiblingElement("objectgroup"))
	{
		for (tinyxml2::XMLElement* obj = e->FirstChildElement("object"); obj != nullptr; obj = obj->NextSiblingElement("object"))
		{
			int tile_id = obj->IntAttribute("gid");
			int x = static_cast<int>(glm::floor(static_cast<float>(obj->IntAttribute("x") + 16.f) / 32.f));
			int y = static_cast<int>(glm::floor(static_cast<float>(obj->IntAttribute("y") - 16.f) / 32.f));

			switch (tile_id)
			{
			case 129: //hero
			{
				auto hero = new Kripta::Hero();
				hero->Place(x, y);
				room.AddChild(hero);
				SetHeroXY(static_cast<float>(x) * 32.f + 16.f, static_cast<float>(y) * 32.f + 16.f);
				hero->SetLevel(2);

				prop_elem = obj->FirstChildElement("properties");

				if (prop_elem)
				for (tinyxml2::XMLElement* prop = prop_elem->FirstChildElement("property"); prop != nullptr; prop = prop->NextSiblingElement("property"))
				{
					ok::String prop_name = prop->Attribute("name");

					/*if (prop_name == "type")
					{
						layer_type = prop->IntAttribute("value");
						layers_counter++;
					}*/
				}
			}
			break;
			case 130: //goblin
			{
				auto goblin = new Kripta::Goblin();
				goblin->Place(x, y);
				room.AddChild(goblin);
				goblin->SetLevel(1);

				prop_elem = obj->FirstChildElement("properties");

				if (prop_elem)
				for (tinyxml2::XMLElement* prop = prop_elem->FirstChildElement("property"); prop != nullptr; prop = prop->NextSiblingElement("property"))
				{
					ok::String prop_name = prop->Attribute("name");

					if (prop_name == "level")
					{
						goblin->SetLevel(prop->IntAttribute("value"));
					}
				}
			}
			break;
			case 145: //gold pile
			{
				auto gold_pile = new Kripta::GoldPile();
				gold_pile->Place(x, y);
				room.AddChild(gold_pile);

				prop_elem = obj->FirstChildElement("properties");

				if (prop_elem)
				for (tinyxml2::XMLElement* prop = prop_elem->FirstChildElement("property"); prop != nullptr; prop = prop->NextSiblingElement("property"))
				{
					ok::String prop_name = prop->Attribute("name");

					if (prop_name == "gold_amount")
					{		
						gold_pile->level = prop->IntAttribute("value");
						//goblin->SetLevel(prop->IntAttribute("value"));
					}
				}
			}
			break;
			}

			
		}
	}

	fov_map->CalculateFOV(static_cast<int>(glm::floor(room.hero_x / 32.f)), static_cast<int>(glm::floor(room.hero_y / 32.f)), 12, true);
}

void Kripta::Game::BlockFloor(int grid_x, int grid_y, Kripta::IObject * owner)
{
	room.objects_grid_ground[grid_x + grid_y * 100] = owner;
}

void Kripta::Game::CreateTombForMe(Kripta::IObject * me)
{
	Kripta::Object* obj = static_cast<Kripta::Object*>(me);

	auto tomb = new Kripta::Tomb();
	tomb->Place(obj->grid_x, obj->grid_y);
	room.AddChild(tomb);
	tomb->SetLevel(obj->level);

	tomb->creature_id = obj->id;
	tomb->turns_to_respawn = 5;
}

void Kripta::Game::PushToPostUpdateList(Kripta::IObject * obj)
{
	_post_update_list.push_back(obj);
}

void Kripta::Game::PushToDeathList(Kripta::IObject * obj)
{
	_death_list.push_back(obj);
}

glm::vec2 Kripta::Game::GetHeroXY()
{
	return glm::vec2(glm::floor(room.hero_x / 32.f),glm::floor(room.hero_y / 32.f));
}

bool Kripta::Game::GetFov(int grid_x, int grid_y)
{
	return fov_map->GetFOV(grid_x, grid_y);
}

int Kripta::Game::TurnStage()
{
	return _turn_stage;
}

void Kripta::Game::BlockGrid(int grid_x, int grid_y, Kripta::IObject* owner)
{
	room.objects_grid[grid_x + grid_y * 100] = owner;
}

void Kripta::Game::SetHeroXY(float x, float y)
{
	room.hero_x = x;
	room.hero_y = y;
	room.camera_x = x - 1024.f*0.5f;
	room.camera_y = y - 768.f*0.5f;
}

Kripta::RoomPickData Kripta::Game::PickRoom(int grid_x, int grid_y)
{
	Kripta::RoomPickData result;

	result.floor_obj = room.objects_grid_ground[grid_x + grid_y * 100];
	result.place_obj = room.objects_grid[grid_x + grid_y * 100];

	switch (room.tiles_layer_0[grid_x + grid_y * 100])
	{
		case 1 :
		{
			//wall
			result.wall = true;
		}
		break;

		case 2 :
		{
			//floor
			result.floor = true;
		}
		break;
	}

	return result;
}
