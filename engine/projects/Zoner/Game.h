#pragma once

#include "..\..\Application.h"
#include "..\..\GameObject.h"

#include "IGame.h"

//include all game classes here
#include "Ship.h"
#include "Space.h"
#include "JumpHole.h"

//include ext classes
#include "Preloader.h"

namespace Zoner
{
	class Game : public ok::Application, public Zoner::IGame
	{
	public:
		Game();
		void Init();
		void Update(float dt);

		void LoadDefaultGame();

		void SetGameName(ok::String game_name);
		void LoadGame();
		void SaveGame();

	protected:
	private:
		void LoadUserSettings();
		void SaveUserSettings();
		
		void ShowPreloader();
		void HidePreloader();

		void LoadGameBegin();
		void LoadGameUpdate();
		void LoadGameEnd();
		int _load_save_game_stage = 0;
		int _load_save_game_step = 0;
		int _load_save_game_step_max = 0;

		Zoner::Preloader _preloader;
		bool _game_states[static_cast<int>(Zoner::GameStates::Count)];

		std::unordered_map<std::string, Zoner::ShipBlueprint*> _ship_blueprints;
		
		//planets, systems, spacecrafts etc here...
		std::unordered_map<std::string, Zoner::Space*> _spaces;
		Zoner::Space* _current_space = nullptr;
		Zoner::Ship* _current_player_ship = nullptr;

		//every step lasts for 24 hours (decisions happens once per step, so once per day)
		unsigned int era = std::numeric_limits<unsigned int>::min();	//from UINT_MIN to UINT_MAX eras (cycled) (named value with "era of <something>) [0 .. UINT_MAX]
		unsigned int millennia = 1;										//1000 millenials per era (named value with "millennia of <something>) [1 .. 1000]
		unsigned int epoch = 1;											//10 epoch per millennia (named value with "epoch of <something>) [1 .. 10]
		unsigned int year = 1;											//100 years per epoch (named value with "year of <something>") [1 .. 100]
		unsigned int month = 1;											//12 month per year (named value) [1 .. 12]
		unsigned int day = 1;											//30 days per month [1 .. 30]
		unsigned int hour = 0;											//24 hours per day [0 .. 24]

		const float real_seconds_per_day = 4.0f;
		float day_progress = 0;
		int daily_updates_done = 0;

		//called everyday at 24 o'clock
		void TimeStep();

		Zoner::GameScreen current_game_screen = Zoner::GameScreen::Space;
		void UpdateGameScreen_Space(float dt);

		ok::graphics::LineBatch* time_bar;

		ok::String _current_game_name = "default";
		tinyxml2::XMLDocument _game_file;
		tinyxml2::XMLElement* _game_file_element;
		tinyxml2::XMLElement* _game_file_element_iterator;
	public:
		int GetScreenWidth();
		int GetScreenHeight();
		std::unordered_map<std::string, Zoner::ShipBlueprint*>& GetShipBlueprints();

		bool StateTrue(Zoner::GameStates state);
		bool StateFalse(Zoner::GameStates state);
		void State(Zoner::GameStates state, bool value);

		Zoner::IShip* GetCurrentPlayerShip();
		void ChangeCurrentLocation(Zoner::ISpace* location);
	};
}