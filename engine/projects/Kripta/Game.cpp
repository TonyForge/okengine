#include "Game.h"

Kripta::Game::Game()
{
	_settings_file_name = "kripta.settings.xml";
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

	LoadRoom("map1.tmx");
}

void Kripta::Game::Update(float dt)
{
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

	camera->BeginTransform(ok::TransformSpace::WorldSpace);
	camera->SetPosition(camera_pos - glm::vec3(100.f, 100.f, 0.f) * dt);
	camera->EndTransform(false);

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


	int tiles_rect_left = static_cast<int>(glm::floor(overlap_rect.GetLeft() / 32.f));
	int tiles_rect_top = static_cast<int>(glm::floor(overlap_rect.GetTop() / 32.f));
	int tiles_rect_width = static_cast<int>(glm::ceil(overlap_rect.GetWidth() / 32.f));
	int tiles_rect_height = static_cast<int>(glm::ceil(overlap_rect.GetHeight() / 32.f));

	if (tiles_rect_width > 99) tiles_rect_width = 99;
	if (tiles_rect_height > 99) tiles_rect_height = 99;


	ok::graphics::SpriteInfo spr_info;
	spr_info.rect = ok::graphics::TextureRect(tex, 0, 0, 32, 32);
	spr_info.hotspot = glm::vec2(0.f, 0.f);


	//render tiles layer 0
	ok::graphics::Camera::PushCamera(pp_camera);
	sprite_batch->BatchBegin(10.f);
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
	sprite_batch->BatchBegin(20.f);
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

	//render tiles layer 2
	ok::graphics::Camera::PushCamera(pp_camera);
	sprite_batch->BatchBegin(30.f);
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


	/*line_batch->BatchBegin();
	line_batch->LineAB(glm::vec3(0.f, 0.f, 0.f), glm::vec3(100.f, 100.f, 0.f));
	line_batch->BatchEnd();*/

	ok::graphics::Camera::PopCamera();
}

ok::graphics::RenderTarget * Kripta::Game::GetScreenBuffer()
{
	return fixed_resolution_framebuffer;
}

void Kripta::Game::LoadRoom(ok::String path)
{
	room.Reset();

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
}

std::pair<Kripta::IObject*, Kripta::IObject*> Kripta::Game::PickObjectFromGrid(int grid_x, int grid_y)
{
	
	Kripta::IObject* ground_obj = room.objects_grid_ground[grid_x + grid_y * 100];
	Kripta::IObject* obj = room.objects_grid[grid_x + grid_y * 100];

	return std::make_pair<Kripta::IObject*, Kripta::IObject*>(ground_obj, obj);
}
