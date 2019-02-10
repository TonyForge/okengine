#pragma once

#include "IGame.h"
#include "GameRoom.h"
#include "Characters.h"
#include "..\..\FOVMap.h"

namespace Kripta
{
	class Game : public Kripta::IGame
	{
	public:
		Game();
		void Init();
		void Update(float dt);
		ok::graphics::RenderTarget* GetScreenBuffer();
		virtual void LoadRoom(ok::String path);
		
		virtual void BlockFloorSpecial(int grid_x, int grid_y, Kripta::IObject* owner);
		virtual void BlockFloor(int grid_x, int grid_y, Kripta::IObject* owner);
		virtual void CreateTombForMe(Kripta::IObject* me);
		virtual void MoveMeToNextFloor(Kripta::IObject* me);

		virtual void PushToPostUpdateList(Kripta::IObject* obj);
		virtual void PushToDeathList(Kripta::IObject* obj);
		virtual glm::vec2 GetHeroXY();
		virtual void SetHeroActionXY(float x, float y);
		virtual glm::vec2 GetHeroActionXY();
		virtual bool GetFov(int grid_x, int grid_y);
		virtual int TurnStage();
		virtual void BlockGrid(int grid_x, int grid_y, Kripta::IObject* owner);
		virtual void SetHeroXY(float x, float y);
		virtual Kripta::RoomPickData PickRoom(int grid_x, int grid_y);

		Kripta::GameRoom room;

		virtual void SaveGame();
		virtual void LoadGame();
		virtual void NewGame();
	private:
		bool _move_hero_to_next_room = false;
		ok::String _next_room_name;

		std::vector<Kripta::IObject*> _post_update_list;
		std::vector<Kripta::IObject*> _death_list;
		int _turn_stage = 0;
		ok::FOVMap* fov_map;

		int _main_menu_requested = 0;
		int _main_menu_item_selected = 0;
		int _save_game_selection_enabled = 0;
		int _load_game_selection_enabled = 0;
		int _save_load_game_item_selected = 0;

		int savegame0_step = -1;
		int savegame1_step = -1;
		int savegame2_step = -1;
		int savegame3_step = -1;
		int savegame4_step = -1;

		ok::String savegame_text;
	};
}