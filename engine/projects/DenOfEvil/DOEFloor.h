#pragma once

#include "..\..\FOVMap.h"
#include "..\..\Rect2D.h"
#include "..\..\SpriteBatch.h"
#include "..\..\AssetsBasic.h"
#include "..\..\Camera.h"

#include "IDOEGame.h"

namespace DOE
{
	class FloorTile
	{
	public:
		int sprite_id;
		std::vector<int> overlays; //3 max
		bool solid;
		bool visited;
		bool hidden;
		ok::GameObject root_corpses;
		ok::GameObject root_scriptables_local;
		ok::GameObject root_items;

		void OnExplore();
		void OnShow();
		void OnHide();
		void OnTeamStepIn(int team_id = 0);
		void OnTeamStepOut(int team_id = 0);
		void OnMapInit();
	};

	class DOEFloor
	{
	public:
		DOEFloor();
		void Render(ok::graphics::SpriteBatch& batch, ok::Rect2Df& viewport);
		void UpdateFOV(float eye_x, float eye_y, ok::Rect2Df viewport);
		DOE::FloorTile& GetTile(int x, int y);
		ok::GameObject* GetRootAll();
	protected:
	private:
		ok::GameObject root_all;
		ok::GameObject root_mobs;
		ok::GameObject root_scriptables_global;
		ok::GameObject root_player;
		

		std::vector<DOE::FloorTile> map;
		ok::FOVMap fov;
		ok::graphics::Texture* tiles_texture = nullptr;
		ok::graphics::Material* _shader_with_grayscale_support = nullptr;
		static std::vector<ok::graphics::SpriteInfo> tiles_info;
	};

	
}