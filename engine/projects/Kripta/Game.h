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
		
		virtual void BlockFloor(int grid_x, int grid_y, Kripta::IObject* owner);
		virtual void CreateTombForMe(Kripta::IObject* me);

		virtual void PushToPostUpdateList(Kripta::IObject* obj);
		virtual void PushToDeathList(Kripta::IObject* obj);
		virtual glm::vec2 GetHeroXY();
		virtual bool GetFov(int grid_x, int grid_y);
		virtual int TurnStage();
		virtual void BlockGrid(int grid_x, int grid_y, Kripta::IObject* owner);
		virtual void SetHeroXY(float x, float y);
		virtual Kripta::RoomPickData PickRoom(int grid_x, int grid_y);

		Kripta::GameRoom room;

		virtual void SaveGame();
		virtual void LoadGame();

	private:
		std::vector<Kripta::IObject*> _post_update_list;
		std::vector<Kripta::IObject*> _death_list;
		int _turn_stage = 0;
		ok::FOVMap* fov_map;

		int _main_menu_item_selected = 0;
		int _save_game_selection_enabled = 0;
		int _load_game_selection_enabled = 0;
		int _save_load_game_item_selected = 0;
	};
}