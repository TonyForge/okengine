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

	gui_camera = new ok::graphics::Camera(ok::graphics::CameraCoordinateSystem::Screen);
	gui_camera->SetProjectionOrtho(1024, 768, 100.f, 1.f);

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

	font_main_menu = ok::AssetsBasic::instance().GetFont("main_menu_font");
	font_main_menu->SetBrushOuterGlow(ok::Color(41,15,0,255), 0.65f, 0.15f);
	font_main_menu->SetBrushBold(0.2f);
	font_main_menu->SetBrushColor(ok::Color::White);
	font_main_menu->SetInternalFont(ok::AssetsBasic::instance().GetInternalFont("consola.font.xml"));

	fov_map = new ok::FOVMap(100, 100);

	room.Reset();
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
		if (main_menu_enabled)
		{
			//do nothing
		}
		else
		{
			if (Kripta::TurnController::turn_members_decision_made == Kripta::TurnController::turn_members_total)
			{
				_turn_stage = 2;
				Kripta::TurnController::turn_in_progress = true;
			}
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

	
	if (Kripta::TurnController::turn_members_ready == 0 && Kripta::TurnController::turn_in_progress == false)
	{
		for (auto obj : _death_list)
		{
			obj->SetParent(nullptr);
			delete obj;
		}

		_death_list.clear();
	}

	_post_update_list.clear();

	if (_turn_stage == 1)
	{
		if (_main_menu_requested)
		{
			_main_menu_requested = 0;
			main_menu_enabled = 1;
			_save_game_selection_enabled = 0;
			_load_game_selection_enabled = 0;
		}

		if (main_menu_enabled)
		{
			if (_save_game_selection_enabled == 1 || _load_game_selection_enabled == 1)
			{
				if (ok::Input::o().KeyPressed(ok::KKey::Escape))
				{
					_save_game_selection_enabled = 0;
					_load_game_selection_enabled = 0;
				}

				if (ok::Input::o().KeyPressed(ok::KKey::Down)) _save_load_game_item_selected++;
				if (ok::Input::o().KeyPressed(ok::KKey::Up)) _save_load_game_item_selected--;

				if (_save_load_game_item_selected > 4) _save_load_game_item_selected = 0;
				if (_save_load_game_item_selected < 0) _save_load_game_item_selected = 4;

				if (ok::Input::o().KeyPressed(ok::KKey::Return))
				{
					if (_save_game_selection_enabled)
					{
						//save game
						SaveGame();
						main_menu_enabled = 0;
						_save_game_selection_enabled = 0;
						_load_game_selection_enabled = 0;
					}

					if (_load_game_selection_enabled)
					{
						//load game
						LoadGame();
						main_menu_enabled = 0;
						_save_game_selection_enabled = 0;
						_load_game_selection_enabled = 0;
					}
				}

				ok::graphics::SpriteInfo menu_selector = ok::AssetsBasic::instance().GetSpriteAtlas("gui.atlas")->Get(ok::String("menu_selector"));

				ok::graphics::Camera::PushCamera(gui_camera);

				sprite_batch->BatchBegin(4.f);

				if (_save_load_game_item_selected == 0)
					sprite_batch->Draw(&menu_selector, glm::vec2(519.f, -270.f), 0.f, glm::vec2(1.f, 1.f));

				if (_save_load_game_item_selected == 1)
					sprite_batch->Draw(&menu_selector, glm::vec2(519.f, -336.f), 0.f, glm::vec2(1.f, 1.f));

				if (_save_load_game_item_selected == 2)
					sprite_batch->Draw(&menu_selector, glm::vec2(519.f, -400.f), 0.f, glm::vec2(1.f, 1.f));

				if (_save_load_game_item_selected == 3)
					sprite_batch->Draw(&menu_selector, glm::vec2(519.f, -464.f), 0.f, glm::vec2(1.f, 1.f));

				if (_save_load_game_item_selected == 4)
					sprite_batch->Draw(&menu_selector, glm::vec2(519.f, -527.f), 0.f, glm::vec2(1.f, 1.f));


				sprite_batch->BatchEnd();

				ok::graphics::Camera::PopCamera();

				text_batch->SetBrushAlignHorizontal(ok::graphics::TextAlign::Center);
				text_batch->SetBrushAlignVertical(ok::graphics::TextAlign::Top);
				text_batch->SetBrushSize(32);
				text_batch->SetBrushPosition(glm::vec2(1024.f, 500.f)*0.5f);
				text_batch->SetBrushFont(font_main_menu);
				text_batch->SetRowSpacingPx(4.f);
				text_batch->SetBrushGradient(ok::Color(255, 205, 0, 255), ok::Color(184, 120, 30, 255));
				text_batch->SetBrushOuterShadow(ok::Color(0, 0, 0, 127), 0.5f, 0.25f, 0.f, 0.1f);
				text_batch->BatchBegin();
				text_batch->Draw(savegame_text);
				text_batch->BatchEnd();
				text_batch->SetRowSpacingPx(0.f);
			}

			if (_save_game_selection_enabled == 0 && _load_game_selection_enabled == 0)
			{
				if (ok::Input::o().KeyPressed(ok::KKey::Down)) _main_menu_item_selected++;
				if (ok::Input::o().KeyPressed(ok::KKey::Up)) _main_menu_item_selected--;

				if (_main_menu_item_selected > 3) _main_menu_item_selected = 0;
				if (_main_menu_item_selected < 0) _main_menu_item_selected = 3;

				if (ok::Input::o().KeyPressed(ok::KKey::Escape))
				{
					main_menu_enabled = 0;
					_save_game_selection_enabled = 0;
					_load_game_selection_enabled = 0;
				}
				else
				if (ok::Input::o().KeyPressed(ok::KKey::Return))
				{
					if (_main_menu_item_selected == 0)
					{
						//new game
						NewGame();
						main_menu_enabled = 0;
						_save_game_selection_enabled = 0;
						_load_game_selection_enabled = 0;
					}

					if (_main_menu_item_selected == 1)
					{
						//save game
						_save_game_selection_enabled = 1;

						savegame_text = "";

						std::ifstream f;
						f.open("savegame0.dat", std::ofstream::binary);
						if (f.fail())
						{
							savegame0_step = -1;
							savegame_text += ok::String(L"-ÏÓÑÒÎ-\n");
						}
						else
						{
							f.read((char*)(&savegame0_step), sizeof(savegame0_step));
							f.close();
							savegame_text += ok::String(std::to_wstring(savegame0_step)) + ok::String(L"\n");
						}
						f.open("savegame1.dat", std::ofstream::binary);
						if (f.fail())
						{
							savegame1_step = -1;
							savegame_text += ok::String(L"-ÏÓÑÒÎ-\n");
						}
						else
						{
							f.read((char*)(&savegame1_step), sizeof(savegame1_step));
							f.close();
							savegame_text += ok::String(std::to_wstring(savegame1_step)) + ok::String(L"\n");
						}
						f.open("savegame2.dat", std::ofstream::binary);
						if (f.fail())
						{
							savegame2_step = -1;
							savegame_text += ok::String(L"-ÏÓÑÒÎ-\n");
						}
						else
						{
							f.read((char*)(&savegame2_step), sizeof(savegame2_step));
							f.close();
							savegame_text += ok::String(std::to_wstring(savegame2_step)) + ok::String(L"\n");
						}
						f.open("savegame3.dat", std::ofstream::binary);
						if (f.fail())
						{
							savegame3_step = -1;
							savegame_text += ok::String(L"-ÏÓÑÒÎ-\n");
						}
						else
						{
							f.read((char*)(&savegame3_step), sizeof(savegame3_step));
							f.close();
							savegame_text += ok::String(std::to_wstring(savegame3_step)) + ok::String(L"\n");
						}
						f.open("savegame4.dat", std::ofstream::binary);
						if (f.fail())
						{
							savegame4_step = -1;
							savegame_text += ok::String(L"-ÏÓÑÒÎ-\n");
						}
						else
						{
							f.read((char*)(&savegame4_step), sizeof(savegame4_step));
							f.close();
							savegame_text += ok::String(std::to_wstring(savegame4_step)) + ok::String(L"\n");
						}
					}

					if (_main_menu_item_selected == 2)
					{
						//load game
						_load_game_selection_enabled = 1;

						savegame_text = "";

						std::ifstream f;
						f.open("savegame0.dat", std::ofstream::binary);
						if (f.fail())
						{
							savegame0_step = -1;
							savegame_text += ok::String(L"-ÏÓÑÒÎ-\n");
						}
						else
						{
							f.read((char*)(&savegame0_step), sizeof(savegame0_step));
							f.close();
							savegame_text += ok::String(std::to_wstring(savegame0_step)) + ok::String(L"\n");
						}
						f.open("savegame1.dat", std::ofstream::binary);
						if (f.fail())
						{
							savegame1_step = -1;
							savegame_text += ok::String(L"-ÏÓÑÒÎ-\n");
						}
						else
						{
							f.read((char*)(&savegame1_step), sizeof(savegame1_step));
							f.close();
							savegame_text += ok::String(std::to_wstring(savegame1_step)) + ok::String(L"\n");
						}
						f.open("savegame2.dat", std::ofstream::binary);
						if (f.fail())
						{
							savegame2_step = -1;
							savegame_text += ok::String(L"-ÏÓÑÒÎ-\n");
						}
						else
						{
							f.read((char*)(&savegame2_step), sizeof(savegame2_step));
							f.close();
							savegame_text += ok::String(std::to_wstring(savegame2_step)) + ok::String(L"\n");
						}
						f.open("savegame3.dat", std::ofstream::binary);
						if (f.fail())
						{
							savegame3_step = -1;
							savegame_text += ok::String(L"-ÏÓÑÒÎ-\n");
						}
						else
						{
							f.read((char*)(&savegame3_step), sizeof(savegame3_step));
							f.close();
							savegame_text += ok::String(std::to_wstring(savegame3_step)) + ok::String(L"\n");
						}
						f.open("savegame4.dat", std::ofstream::binary);
						if (f.fail())
						{
							savegame4_step = -1;
							savegame_text += ok::String(L"-ÏÓÑÒÎ-\n");
						}
						else
						{
							f.read((char*)(&savegame4_step), sizeof(savegame4_step));
							f.close();
							savegame_text += ok::String(std::to_wstring(savegame4_step)) + ok::String(L"\n");
						}
					}

					if (_main_menu_item_selected == 3)
					{
						//exit
					}
				}

				ok::graphics::SpriteInfo menu_selector = ok::AssetsBasic::instance().GetSpriteAtlas("gui.atlas")->Get(ok::String("menu_selector"));

				ok::graphics::Camera::PushCamera(gui_camera);

				sprite_batch->BatchBegin(4.f);

				if (_main_menu_item_selected == 0)
					sprite_batch->Draw(&menu_selector, glm::vec2(519.f, -270.f), 0.f, glm::vec2(1.f, 1.f));

				if (_main_menu_item_selected == 1)
					sprite_batch->Draw(&menu_selector, glm::vec2(519.f, -336.f), 0.f, glm::vec2(1.f, 1.f));

				if (_main_menu_item_selected == 2)
					sprite_batch->Draw(&menu_selector, glm::vec2(519.f, -400.f), 0.f, glm::vec2(1.f, 1.f));

				if (_main_menu_item_selected == 3)
					sprite_batch->Draw(&menu_selector, glm::vec2(519.f, -464.f), 0.f, glm::vec2(1.f, 1.f));

				sprite_batch->BatchEnd();

				ok::graphics::Camera::PopCamera();

				text_batch->SetBrushAlignHorizontal(ok::graphics::TextAlign::Center);
				text_batch->SetBrushAlignVertical(ok::graphics::TextAlign::Top);
				text_batch->SetBrushSize(32);
				text_batch->SetBrushPosition(glm::vec2(1024.f, 500.f)*0.5f);
				text_batch->SetBrushFont(font_main_menu);
				text_batch->SetRowSpacingPx(4.f);
				text_batch->SetBrushGradient(ok::Color(255, 205, 0, 255), ok::Color(184, 120, 30, 255));
				text_batch->SetBrushOuterShadow(ok::Color(0, 0, 0, 127), 0.5f, 0.25f, 0.f, 0.1f);
				text_batch->BatchBegin();
				text_batch->Draw(ok::String(L"ÍÎÂÀß ÈÃÐÀ\nÑÎÕÐÀÍÈÒÜ\nÇÀÃÐÓÇÈÒÜ\nÂÛÕÎÄ"));
				text_batch->BatchEnd();
				text_batch->SetRowSpacingPx(0.f);
				//process main menu
			}
			
		}
		else
		{
			if (ok::Input::o().KeyPressed(ok::KKey::Escape))
			{
				main_menu_enabled = 1;
				_save_game_selection_enabled = 0;
				_load_game_selection_enabled = 0;
			}
		}
	}
	else
	{
		if (ok::Input::o().KeyPressed(ok::KKey::Escape))
		{
			_main_menu_requested = 1;
		}
	}
}

ok::graphics::RenderTarget * Kripta::Game::GetScreenBuffer()
{
	return fixed_resolution_framebuffer;
}

void Kripta::Game::LoadRoom(ok::String path)
{

	Kripta::TurnController::turn_members_decision_made = 0;

	for (int y = 0; y < 100; y++)
	{
		for (int x = 0; x < 100; x++)
		{
			if (room.objects_grid[x + y * 100] != nullptr)
			{
				room.objects_grid[x + y * 100]->SetParent(nullptr);
				delete (Kripta::Object*)room.objects_grid[x + y * 100];
			}

			if (room.objects_grid_ground[x + y * 100] != nullptr)
			{
				room.objects_grid_ground[x + y * 100]->SetParent(nullptr);
				delete (Kripta::Object*)room.objects_grid_ground[x + y * 100];
			}

			if (room.objects_grid_ground_special[x + y * 100] != nullptr)
			{
				room.objects_grid_ground_special[x + y * 100]->SetParent(nullptr);
				delete (Kripta::Object*)room.objects_grid_ground_special[x + y * 100];
			}
		}
	}

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

						if (tile_id == 1 || tile_id == 5 || tile_id == 6)
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
				hero->SetLevel(hero_level);
				hero->hp = hero_hp;
				hero->gold = hero_gold;
				hero->gold_to_levelup = hero_gold_to_levelup;

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
			case 146: //gold pile
			{
				auto health_potion = new Kripta::HealthPotion();
				health_potion->Place(x, y);
				room.AddChild(health_potion);
			}
			break;
			case 131: //golden guard
			{
				auto golden_gruard = new Kripta::GoldenGuard();
				golden_gruard->Place(x, y);
				room.AddChild(golden_gruard);
				golden_gruard->SetLevel(1);
				golden_gruard->home_grid_xy = glm::vec2(x, y);
				prop_elem = obj->FirstChildElement("properties");

				if (prop_elem)
					for (tinyxml2::XMLElement* prop = prop_elem->FirstChildElement("property"); prop != nullptr; prop = prop->NextSiblingElement("property"))
					{
						ok::String prop_name = prop->Attribute("name");

						if (prop_name == "level")
						{
							golden_gruard->SetLevel(prop->IntAttribute("value"));
						}
					}
			}
			break;
			}

			
		}
	}

	fov_map->CalculateFOV(static_cast<int>(glm::floor(room.hero_x / 32.f)), static_cast<int>(glm::floor(room.hero_y / 32.f)), 12, true);
}

void Kripta::Game::BlockFloorSpecial(int grid_x, int grid_y, Kripta::IObject * owner)
{
	room.objects_grid_ground_special[grid_x + grid_y * 100] = owner;
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

void Kripta::Game::SaveGame()
{
	std::ofstream f;

	if (_save_load_game_item_selected == 0)
		f.open("savegame0.dat", std::ofstream::binary);

	if (_save_load_game_item_selected == 1)
		f.open("savegame1.dat", std::ofstream::binary);

	if (_save_load_game_item_selected == 2)
		f.open("savegame2.dat", std::ofstream::binary);

	if (_save_load_game_item_selected == 3)
		f.open("savegame3.dat", std::ofstream::binary);

	if (_save_load_game_item_selected == 4)
		f.open("savegame4.dat", std::ofstream::binary);

	f.write((char*)(&turn_number), sizeof(turn_number));
	
	f.write((char*)room.tiles_layer_0, sizeof(room.tiles_layer_0));
	f.write((char*)room.tiles_layer_1, sizeof(room.tiles_layer_0));
	f.write((char*)room.tiles_layer_2, sizeof(room.tiles_layer_0));

	for (int y = 0; y < 100; y++)
	{
		for (int x = 0; x < 100; x++)
		{
			auto fov_data = fov_map->Get(x, y);
			auto isfov = fov_data.IsFov();
			auto iswalkable = fov_data.IsWalkable();
			auto istransparent = fov_data.IsTransparent();

			f.write((char*)&isfov, sizeof(isfov));
			f.write((char*)&iswalkable, sizeof(iswalkable));
			f.write((char*)&istransparent, sizeof(istransparent));
		}
	}

	char data_block[4 * 20];

	for (int y = 0; y < 100; y++)
	{
		for (int x = 0; x < 100; x++)
		{
			Kripta::Object* obj = (Kripta::Object*)room.objects_grid[x + y * 100];

			memset(data_block, 0, sizeof(data_block));

			if (obj != nullptr)
			{
				memcpy(&data_block[4 * 0], (char*)(&(obj->id)), 4);
				memcpy(&data_block[4 * 1], (char*)(&(obj->grid_x)), 4);
				memcpy(&data_block[4 * 2], (char*)(&(obj->grid_y)), 4);
				memcpy(&data_block[4 * 3], (char*)(&(obj->level)), 4);
				memcpy(&data_block[4 * 4], (char*)(&(obj->hp)), 4);
				memcpy(&data_block[4 * 5], (char*)(&(obj->action_id)), 4);
				memcpy(&data_block[4 * 6], (char*)(&(obj->action_grid_x)), 4);
				memcpy(&data_block[4 * 7], (char*)(&(obj->action_grid_y)), 4);
				memcpy(&data_block[4 * 8], (char*)(&(obj->last_seen_hero_xy.x)), 4);
				memcpy(&data_block[4 * 9], (char*)(&(obj->last_seen_hero_xy.y)), 4);

				//custom fields
				if (obj->id == Kripta::ObjectID::Hero)
				{
					Kripta::Hero* _obj = (Kripta::Hero*)obj;
					memcpy(&data_block[4 * 10], (char*)(&(_obj->gold)), 4);
					memcpy(&data_block[4 * 11], (char*)(&(_obj->gold_to_levelup)), 4);
				}

				if (obj->id == Kripta::ObjectID::GoldenGuard)
				{
					Kripta::GoldenGuard* _obj = (Kripta::GoldenGuard*)obj;
					memcpy(&data_block[4 * 10], (char*)(&(_obj->home_grid_xy.x)), 4);
					memcpy(&data_block[4 * 11], (char*)(&(_obj->home_grid_xy.y)), 4);
				}
			}
			else
			{
				int empty_id = -1;
				memcpy(&data_block[4 * 0], (char*)(&(empty_id)), 4);
			}

			f.write(data_block, sizeof(data_block));
		}
	}

	for (int y = 0; y < 100; y++)
	{
		for (int x = 0; x < 100; x++)
		{
			Kripta::Object* obj = (Kripta::Object*)room.objects_grid_ground[x + y * 100];

			memset(data_block, 0, sizeof(data_block));

			if (obj != nullptr)
			{
				memcpy(&data_block[4 * 0], (char*)(&(obj->id)), 4);
				memcpy(&data_block[4 * 1], (char*)(&(obj->grid_x)), 4);
				memcpy(&data_block[4 * 2], (char*)(&(obj->grid_y)), 4);
				memcpy(&data_block[4 * 3], (char*)(&(obj->level)), 4);
				memcpy(&data_block[4 * 4], (char*)(&(obj->hp)), 4);
				memcpy(&data_block[4 * 5], (char*)(&(obj->action_id)), 4);
				memcpy(&data_block[4 * 6], (char*)(&(obj->action_grid_x)), 4);
				memcpy(&data_block[4 * 7], (char*)(&(obj->action_grid_y)), 4);
				memcpy(&data_block[4 * 8], (char*)(&(obj->last_seen_hero_xy.x)), 4);
				memcpy(&data_block[4 * 9], (char*)(&(obj->last_seen_hero_xy.y)), 4);
			}
			else
			{
				int empty_id = -1;
				memcpy(&data_block[4 * 0], (char*)(&(empty_id)), 4);
			}

			f.write(data_block, sizeof(data_block));
		}
	}

	for (int y = 0; y < 100; y++)
	{
		for (int x = 0; x < 100; x++)
		{
			Kripta::Object* obj = (Kripta::Object*)room.objects_grid_ground_special[x + y * 100];

			memset(data_block, 0, sizeof(data_block));

			if (obj != nullptr)
			{
				memcpy(&data_block[4 * 0], (char*)(&(obj->id)), 4);
				memcpy(&data_block[4 * 1], (char*)(&(obj->grid_x)), 4);
				memcpy(&data_block[4 * 2], (char*)(&(obj->grid_y)), 4);
				memcpy(&data_block[4 * 3], (char*)(&(obj->level)), 4);
				memcpy(&data_block[4 * 4], (char*)(&(obj->hp)), 4);
				memcpy(&data_block[4 * 5], (char*)(&(obj->action_id)), 4);
				memcpy(&data_block[4 * 6], (char*)(&(obj->action_grid_x)), 4);
				memcpy(&data_block[4 * 7], (char*)(&(obj->action_grid_y)), 4);
				memcpy(&data_block[4 * 8], (char*)(&(obj->last_seen_hero_xy.x)), 4);
				memcpy(&data_block[4 * 9], (char*)(&(obj->last_seen_hero_xy.y)), 4);

				//custom fields
				if (obj->id == Kripta::ObjectID::Tomb)
				{
					Kripta::Tomb* _obj = (Kripta::Tomb*)obj;
					memcpy(&data_block[4 * 10], (char*)(&(_obj->creature_id)), 4);
					memcpy(&data_block[4 * 11], (char*)(&(_obj->initial_turn)), 4);
					memcpy(&data_block[4 * 12], (char*)(&(_obj->turns_to_respawn)), 4);
				}
			}
			else
			{
				int empty_id = -1;
				memcpy(&data_block[4 * 0], (char*)(&(empty_id)), 4);
			}

			f.write(data_block, sizeof(data_block));
		}
	}
	

	f.close();
}

void Kripta::Game::LoadGame()
{
	std::ifstream f;

	if (_save_load_game_item_selected == 0)
		f.open("savegame0.dat", std::ofstream::binary);

	if (_save_load_game_item_selected == 1)
		f.open("savegame1.dat", std::ofstream::binary);

	if (_save_load_game_item_selected == 2)
		f.open("savegame2.dat", std::ofstream::binary);

	if (_save_load_game_item_selected == 3)
		f.open("savegame3.dat", std::ofstream::binary);

	if (_save_load_game_item_selected == 4)
		f.open("savegame4.dat", std::ofstream::binary);

	if (f.fail()) return;

	Kripta::TurnController::turn_members_decision_made = 0;

	for (int y = 0; y < 100; y++)
	{
		for (int x = 0; x < 100; x++)
		{
			if (room.objects_grid[x + y * 100] != nullptr)
			{
				room.objects_grid[x + y * 100]->SetParent(nullptr);
				delete (Kripta::Object*)room.objects_grid[x + y * 100];
			}

			if (room.objects_grid_ground[x + y * 100] != nullptr)
			{
				room.objects_grid_ground[x + y * 100]->SetParent(nullptr);
				delete (Kripta::Object*)room.objects_grid_ground[x + y * 100];
			}

			if (room.objects_grid_ground_special[x + y * 100] != nullptr)
			{
				room.objects_grid_ground_special[x + y * 100]->SetParent(nullptr);
				delete (Kripta::Object*)room.objects_grid_ground_special[x + y * 100];
			}
		}
	}

	room.Reset();
	fov_map->Clear();

	f.read((char*)(&turn_number), sizeof(turn_number));

	f.read((char*)room.tiles_layer_0, sizeof(room.tiles_layer_0));
	f.read((char*)room.tiles_layer_1, sizeof(room.tiles_layer_0));
	f.read((char*)room.tiles_layer_2, sizeof(room.tiles_layer_0));

	for (int y = 0; y < 100; y++)
	{
		for (int x = 0; x < 100; x++)
		{
			auto fov_data = fov_map->Get(x, y);
			auto isfov = fov_data.IsFov();
			auto iswalkable = fov_data.IsWalkable();
			auto istransparent = fov_data.IsTransparent();

			f.read((char*)&isfov, sizeof(isfov));
			f.read((char*)&iswalkable, sizeof(iswalkable));
			f.read((char*)&istransparent, sizeof(istransparent));

			fov_map->SetWalkable(x, y, iswalkable);
			fov_map->SetTransparent(x, y, istransparent);
		}
	}

	char data_block[4 * 20];
	Kripta::Object tmp_obj;

	for (int y = 0; y < 100; y++)
	{
		for (int x = 0; x < 100; x++)
		{
			Kripta::Object* obj = (Kripta::Object*)room.objects_grid[x + y * 100];
			if (obj != nullptr)
			{
				delete obj;
				room.objects_grid[x + y * 100] = nullptr;
			}
				
			f.read(data_block, sizeof(data_block));

			memcpy((char*)(&(tmp_obj.id)), &data_block[4 * 0], 4);
			memcpy((char*)(&(tmp_obj.grid_x)), &data_block[4 * 1], 4);
			memcpy((char*)(&(tmp_obj.grid_y)), &data_block[4 * 2], 4);
			memcpy((char*)(&(tmp_obj.level)), &data_block[4 * 3], 4);
			memcpy((char*)(&(tmp_obj.hp)), &data_block[4 * 4], 4);
			memcpy((char*)(&(tmp_obj.action_id)), &data_block[4 * 5], 4);
			memcpy((char*)(&(tmp_obj.action_grid_x)), &data_block[4 * 6], 4);
			memcpy((char*)(&(tmp_obj.action_grid_y)), &data_block[4 * 7], 4);
			memcpy((char*)(&(tmp_obj.last_seen_hero_xy.x)),&data_block[4 * 8],  4);
			memcpy((char*)(&(tmp_obj.last_seen_hero_xy.y)), &data_block[4 * 9],  4);

			if ((int)tmp_obj.id == -1) continue;

			if (tmp_obj.id == Kripta::ObjectID::Hero)
			{
				auto hero = new Kripta::Hero();
				hero->Place(tmp_obj.grid_x, tmp_obj.grid_y);
				room.AddChild(hero);
				SetHeroXY(static_cast<float>(tmp_obj.grid_x) * 32.f + 16.f, static_cast<float>(tmp_obj.grid_y) * 32.f + 16.f);
				hero->SetLevel(tmp_obj.level);
				hero->hp = tmp_obj.hp;
				hero->action_id = tmp_obj.action_id;
				hero->action_grid_x = tmp_obj.action_grid_x;
				hero->action_grid_y = tmp_obj.action_grid_y;
				hero->last_seen_hero_xy = tmp_obj.last_seen_hero_xy;

				memcpy((char*)(&(hero->gold)), &data_block[4 * 10],  4);
				memcpy((char*)(&(hero->gold_to_levelup)), &data_block[4 * 11], 4);

				hero_gold = hero->gold;
				hero_gold_to_levelup = hero->gold_to_levelup;
				hero_hp = hero->hp;
				hero_level = hero->level;

				obj = hero;
			}

			if (tmp_obj.id == Kripta::ObjectID::Goblin)
			{
				auto goblin = new Kripta::Goblin();
				goblin->Place(tmp_obj.grid_x, tmp_obj.grid_y);
				room.AddChild(goblin);
				goblin->SetLevel(tmp_obj.level);
				goblin->hp = tmp_obj.hp;
				goblin->action_id = tmp_obj.action_id;
				goblin->action_grid_x = tmp_obj.action_grid_x;
				goblin->action_grid_y = tmp_obj.action_grid_y;
				goblin->last_seen_hero_xy = tmp_obj.last_seen_hero_xy;

				obj = goblin;
			}

			if (tmp_obj.id == Kripta::ObjectID::GoldenGuard)
			{
				auto golden_guard = new Kripta::GoldenGuard();
				golden_guard->Place(tmp_obj.grid_x, tmp_obj.grid_y);
				room.AddChild(golden_guard);
				golden_guard->SetLevel(tmp_obj.level);
				golden_guard->hp = tmp_obj.hp;
				golden_guard->action_id = tmp_obj.action_id;
				golden_guard->action_grid_x = tmp_obj.action_grid_x;
				golden_guard->action_grid_y = tmp_obj.action_grid_y;
				golden_guard->last_seen_hero_xy = tmp_obj.last_seen_hero_xy;

				memcpy((char*)(&(golden_guard->home_grid_xy.x)), &data_block[4 * 10], 4);
				memcpy((char*)(&(golden_guard->home_grid_xy.y)), &data_block[4 * 11], 4);

				obj = golden_guard;
			}

			room.objects_grid[x + y * 100] = obj;
		}
	}

	for (int y = 0; y < 100; y++)
	{
		for (int x = 0; x < 100; x++)
		{
			Kripta::Object* obj = (Kripta::Object*)room.objects_grid_ground[x + y * 100];
			if (obj != nullptr)
			{
				delete obj;
				room.objects_grid_ground[x + y * 100] = nullptr;
			}

			f.read(data_block, sizeof(data_block));

			memcpy((char*)(&(tmp_obj.id)), &data_block[4 * 0], 4);
			memcpy((char*)(&(tmp_obj.grid_x)), &data_block[4 * 1], 4);
			memcpy((char*)(&(tmp_obj.grid_y)), &data_block[4 * 2], 4);
			memcpy((char*)(&(tmp_obj.level)), &data_block[4 * 3], 4);
			memcpy((char*)(&(tmp_obj.hp)), &data_block[4 * 4], 4);
			memcpy((char*)(&(tmp_obj.action_id)), &data_block[4 * 5], 4);
			memcpy((char*)(&(tmp_obj.action_grid_x)), &data_block[4 * 6], 4);
			memcpy((char*)(&(tmp_obj.action_grid_y)), &data_block[4 * 7], 4);
			memcpy((char*)(&(tmp_obj.last_seen_hero_xy.x)), &data_block[4 * 8], 4);
			memcpy((char*)(&(tmp_obj.last_seen_hero_xy.y)), &data_block[4 * 9], 4);

			if ((int)tmp_obj.id == -1) continue;

			if (tmp_obj.id == Kripta::ObjectID::GoldPile)
			{
				auto gold_pile = new Kripta::GoldPile();
				gold_pile->Place(tmp_obj.grid_x, tmp_obj.grid_y);
				room.AddChild(gold_pile);
				gold_pile->SetLevel(tmp_obj.level);
				gold_pile->hp = tmp_obj.hp;
				gold_pile->action_id = tmp_obj.action_id;
				gold_pile->action_grid_x = tmp_obj.action_grid_x;
				gold_pile->action_grid_y = tmp_obj.action_grid_y;
				gold_pile->last_seen_hero_xy = tmp_obj.last_seen_hero_xy;

				obj = gold_pile;
			}

			if (tmp_obj.id == Kripta::ObjectID::HealthPotion)
			{
				auto health_potion = new Kripta::HealthPotion();
				health_potion->Place(tmp_obj.grid_x, tmp_obj.grid_y);
				room.AddChild(health_potion);
				health_potion->SetLevel(tmp_obj.level);
				health_potion->hp = tmp_obj.hp;
				health_potion->action_id = tmp_obj.action_id;
				health_potion->action_grid_x = tmp_obj.action_grid_x;
				health_potion->action_grid_y = tmp_obj.action_grid_y;
				health_potion->last_seen_hero_xy = tmp_obj.last_seen_hero_xy;

				obj = health_potion;
			}

			room.objects_grid_ground[x + y * 100] = obj;
		}
	}

	for (int y = 0; y < 100; y++)
	{
		for (int x = 0; x < 100; x++)
		{
			Kripta::Object* obj = (Kripta::Object*)room.objects_grid_ground_special[x + y * 100];
			if (obj != nullptr)
			{
				delete obj;
				room.objects_grid_ground_special[x + y * 100] = nullptr;
			}

			f.read(data_block, sizeof(data_block));

			memcpy((char*)(&(tmp_obj.id)), &data_block[4 * 0], 4);
			memcpy((char*)(&(tmp_obj.grid_x)), &data_block[4 * 1], 4);
			memcpy((char*)(&(tmp_obj.grid_y)), &data_block[4 * 2], 4);
			memcpy((char*)(&(tmp_obj.level)), &data_block[4 * 3], 4);
			memcpy((char*)(&(tmp_obj.hp)), &data_block[4 * 4], 4);
			memcpy((char*)(&(tmp_obj.action_id)), &data_block[4 * 5], 4);
			memcpy((char*)(&(tmp_obj.action_grid_x)), &data_block[4 * 6], 4);
			memcpy((char*)(&(tmp_obj.action_grid_y)), &data_block[4 * 7], 4);
			memcpy((char*)(&(tmp_obj.last_seen_hero_xy.x)), &data_block[4 * 8], 4);
			memcpy((char*)(&(tmp_obj.last_seen_hero_xy.y)), &data_block[4 * 9], 4);

			if ((int)tmp_obj.id == -1) continue;

			if (tmp_obj.id == Kripta::ObjectID::Tomb)
			{
				auto tomb = new Kripta::Tomb();
				tomb->Place(tmp_obj.grid_x, tmp_obj.grid_y);
				room.AddChild(tomb);
				tomb->SetLevel(tmp_obj.level);
				tomb->hp = tmp_obj.hp;
				tomb->action_id = tmp_obj.action_id;
				tomb->action_grid_x = tmp_obj.action_grid_x;
				tomb->action_grid_y = tmp_obj.action_grid_y;
				tomb->last_seen_hero_xy = tmp_obj.last_seen_hero_xy;

				memcpy((char*)(&(tomb->creature_id)), &data_block[4 * 10], 4);
				memcpy((char*)(&(tomb->initial_turn)), &data_block[4 * 11], 4);
				memcpy((char*)(&(tomb->turns_to_respawn)), &data_block[4 * 12], 4);

				obj = tomb;
			}

			room.objects_grid_ground_special[x + y * 100] = obj;
		}
	}


	f.close();

	fov_map->CalculateFOV(static_cast<int>(glm::floor(room.hero_x / 32.f)), static_cast<int>(glm::floor(room.hero_y / 32.f)), 12, true);
}

void Kripta::Game::NewGame()
{
	turn_number = 0;

	hero_gold = 0;
	hero_gold_to_levelup = 1000;
	hero_level = 2;
	hero_hp = 6;

	LoadRoom("map1.tmx");
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

		case 5:
		{
			//stair
			result.stair = true;
		}
		break;

		case 6:
		{
			//door
			result.door = true;
		}
		break;
	}

	return result;
}
