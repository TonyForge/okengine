#include "DOEFloor.h"

std::vector<ok::graphics::SpriteInfo> DOE::DOEFloor::tiles_info;

DOE::DOEFloor::DOEFloor() : fov(global.g_screen_width/global.g_tile_size+2, global.g_screen_height / global.g_tile_size+2)
{
	map.resize(DOE::global.g_floor_size*DOE::global.g_floor_size);

	DOE::FloorTile empty_tile;
	empty_tile.sprite_id = -1;
	empty_tile.solid = false;
	empty_tile.visited = false;
	empty_tile.hidden = true;
	//empty_tile.sprite_id = 1;
	//empty_tile.solid = true;

	std::fill(map.begin(), map.end(), empty_tile);

	fov.Clear();

	root_mobs.Rename("root_mobs");
	root_scriptables_global.Rename("root_scriptables_global");
	root_player.Rename("root_player");

	root_all.AddChild(&root_mobs);
	root_all.AddChild(&root_scriptables_global);
	root_all.AddChild(&root_player);
}

void DOE::DOEFloor::Render(ok::graphics::SpriteBatch & batch, ok::Rect2Df & viewport)
{
	int fov_shift_x = -1 + static_cast<int>(glm::floor(viewport.GetLeft() / DOE::global.g_tile_size));
	int fov_shift_y = -1 + static_cast<int>(glm::floor(viewport.GetTop() / DOE::global.g_tile_size));

	if (tiles_texture == nullptr)
	{
		_shader_with_grayscale_support = ok::AssetsBasic::instance().GetMaterial("SpriteBatch.DOEMapMaterial.xml");
		tiles_texture = ok::AssetsBasic::instance().GetTexture("tiles.png");

		ok::graphics::SpriteInfo info;

		info.hotspot = glm::vec2(0.f, 0.f);
		info.tint_color = ok::Color::White;
		info.tint_power = 0.f;
		info.scale = glm::vec2(1.f, 1.f);

		for (int y = 0; y < (tiles_texture->GetSize().y / global.g_tile_size); y++)
		for (int x = 0; x < (tiles_texture->GetSize().x / global.g_tile_size); x++)
		{
			info.rect = ok::graphics::TextureRect(tiles_texture, x*global.g_tile_size, y*global.g_tile_size, global.g_tile_size, global.g_tile_size);
			DOE::DOEFloor::tiles_info.push_back(info);
		}
	}
		

	ok::Rect2Df map_rectf;
	map_rectf.SetXYWH(
		0.f,
		0.f,
		static_cast<float>(global.g_tile_size*global.g_floor_size),
		static_cast<float>(global.g_tile_size*global.g_floor_size));

	ok::Rect2Df regionf = viewport.GetOverlap(map_rectf);
	regionf.SetXYWH(
		regionf.GetX() / global.g_tile_size,
		regionf.GetY() / global.g_tile_size,
		regionf.GetWidth() / global.g_tile_size,
		regionf.GetHeight() / global.g_tile_size);

	ok::Rect2Di regioni;
	regioni.SetLTRB(
		static_cast<int>(glm::floor(regionf.GetLeft())),
		static_cast<int>(glm::floor(regionf.GetTop())),
		static_cast<int>(glm::ceil(regionf.GetRight())),
		static_cast<int>(glm::ceil(regionf.GetBottom())));

	ok::graphics::Camera camera(ok::graphics::CameraCoordinateSystem::Screen);
	ok::graphics::Camera::PushCamera(&camera);

	camera.SetProjectionOrtho(global.g_screen_width_f, global.g_screen_height_f, 1.f, 100.f);

	camera.BeginTransform();
	camera.SetPosition(
		glm::vec3(
			glm::floor(viewport.GetX()),
			glm::floor(viewport.GetY()),
			0.f)
	);
	camera.EndTransform(true);

	batch.SetMaterial(_shader_with_grayscale_support);
	batch.BatchBegin(10.f);

	for (int y = regioni.GetY(); y < regioni.GetBottom(); y++)
	for (int x = regioni.GetX(); x < regioni.GetRight(); x++)
	{
		DOE::FloorTile& tile = map[x + y * global.g_floor_size];
		bool in_fov = fov.GetFOV(x-fov_shift_x, y-fov_shift_y);
		bool is_visited = tile.visited;

		if (tile.sprite_id == -1 || !is_visited)
		{
			//skip
			/*if (in_fov && !is_visited)
			{
				tile.visited = true;
			}*/
			
		}
		else
		{
			ok::graphics::SpriteInfo& info = tiles_info[tile.sprite_id];

			if (in_fov)
			{
				info.tint_power = 0.f;
			}
			else
			{
				info.tint_power = 1.f;
			}

			batch.Blit(&info, x*global.g_tile_size, y*global.g_tile_size);

			for (auto&& overlay_id : tile.overlays)
			{
				if (overlay_id != -1)
				{
					ok::graphics::SpriteInfo& oinfo = tiles_info[overlay_id];
					oinfo.tint_power = info.tint_power;
					batch.Blit(&oinfo, x*global.g_tile_size, y*global.g_tile_size);
				}
			}
		}
	}

	batch.BatchEnd();
	batch.SetMaterial(nullptr);

	ok::graphics::Camera::PopCamera();
}

void DOE::DOEFloor::UpdateFOV(float eye_x, float eye_y, ok::Rect2Df viewport)
{
	ok::Rect2Di vpi;
	vpi.SetLTRB(
		-1+static_cast<int>(glm::floor(viewport.GetLeft() / global.g_tile_size)),
		-1+static_cast<int>(glm::floor(viewport.GetTop() / global.g_tile_size)),
		1+static_cast<int>(glm::floor(viewport.GetRight() / global.g_tile_size)),
		1+static_cast<int>(glm::floor(viewport.GetBottom() / global.g_tile_size))
		);

	ok::Rect2Di world_rect;
	world_rect.SetLTRB(0, 0, DOE::global.g_floor_size - 1, DOE::global.g_floor_size - 1);

	for (int y = 0; y < vpi.GetHeight(); y++)
	for (int x = 0; x < vpi.GetWidth(); x++)
	{
		if (world_rect.Contains(
			static_cast<float>(vpi.GetLeft() + x),
			static_cast<float>(vpi.GetTop() + y)
		))
		{
			auto & tile = map[(vpi.GetLeft() + x) + (vpi.GetTop() + y) * global.g_floor_size];
			
			if (tile.solid)
			{
				fov.SetWalkable(x, y, false);
				fov.SetTransparent(x, y, false);
			}
			else
			{
				fov.SetWalkable(x, y, true);
				fov.SetTransparent(x, y, true);
			}
		}
		else
		{
			fov.SetWalkable(x, y, false);
			fov.SetTransparent(x, y, false);
		}
	}

	float fov_eye_x = (eye_x - vpi.GetLeft()*global.g_tile_size) / global.g_tile_size;
	float fov_eye_y = (eye_y - vpi.GetTop()*global.g_tile_size) / global.g_tile_size;

	fov_eye_x = glm::floor(fov_eye_x);
	fov_eye_y = glm::floor(fov_eye_y);

	if (fov_eye_x >= 0 && fov_eye_y >= 0 &&
		fov_eye_x < (global.g_screen_width / global.g_tile_size + 2) &&
		fov_eye_y < (global.g_screen_height / global.g_tile_size + 2))
	{
		fov.CalculateFOV(static_cast<int>(fov_eye_x), static_cast<int>(fov_eye_y), 8, true);
	}

	{
		int x = 0;
		int y = 0;
		int fov_x = 0;
		int fov_y = 0;
		for (auto& tile : map)
		{
			fov_x = x - vpi.GetLeft();
			fov_y = y - vpi.GetTop();

			bool in_fov = fov.GetFOV(fov_x, fov_y);

			//if tile in fov and hidden OnShow
			//if tile in fov and not hidden skip
			//if tile not in fov and hidden skip
			//if tile not in fov and not hidden OnHide
			if (tile.hidden)
			{
				if (in_fov)
				{
					if (tile.visited == false)
					{
						tile.visited = true;
						tile.OnExplore();
					}
					
					tile.hidden = false;
					tile.OnShow();
				}
				else
				{
					//skip
				}
			}
			else
			{
				if (in_fov)
				{
					//skip
				}
				else
				{
					tile.OnHide();
					tile.hidden = true;
				}
			}

			x++;
			if (x == global.g_floor_size)
			{
				x = 0;
				y++;
			}
		}
	}
	
}

DOE::FloorTile & DOE::DOEFloor::GetTile(int x, int y)
{
	return map[x + y * DOE::global.g_floor_size];
}

ok::GameObject * DOE::DOEFloor::GetRootAll()
{
	return &root_all;
}

void DOE::FloorTile::OnExplore()
{
}

void DOE::FloorTile::OnShow()
{
}

void DOE::FloorTile::OnHide()
{
}

void DOE::FloorTile::OnTeamStepIn(int team_id)
{
}

void DOE::FloorTile::OnTeamStepOut(int team_id)
{
}

void DOE::FloorTile::OnMapInit()
{
}
