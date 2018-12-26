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

	camera->BeginTransform(ok::TransformSpace::WorldSpace);
	glm::vec3 camera_pos = camera->GetPosition();
	camera->EndTransform(false);

	ok::Rect2Df camera_rect;
	ok::Rect2Df room_rect;

	camera_rect.SetLTRB(camera_pos.x, camera_pos.y, camera_pos.x + 1024.f, camera_pos.y + 768.f);
	room_rect.SetLTRB(0.f, 0.f, 100.f*32.f, 100.f*32.f);

	room_rect.

	//render tiles layer 0
	sprite_batch->BatchBegin(10.f);
	//sprite_batch->Blit()
	//sprite_batch->Draw(tex, glm::vec2(0.f, 0.f), glm::vec2(512.f,512.f),false,glm::vec2(0.f,0.f));



	sprite_batch->BatchEnd();


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
