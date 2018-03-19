#pragma once

#include "..\..\Application.h"
#include "..\..\GameObject.h"

#include "IGame.h"

//include all game classes here
#include "Ship.h"

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

	protected:
	private:
		void ShowPreloader();
		void HidePreloader();

		ok::GameObject _scene_root;
		Zoner::Preloader _preloader;
		bool _game_states[static_cast<int>(Zoner::GameStates::Count)];

		std::unordered_map<std::string, Zoner::ShipBlueprint*> _ship_blueprints;

		//planets, systems, spacecrafts etc here...

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

		//called everyday at 24 o'clock
		void TimeStep();

		ok::graphics::LineBatch* time_bar;

	public:
		int GetScreenWidth();
		int GetScreenHeight();
		std::unordered_map<std::string, Zoner::ShipBlueprint*>& GetShipBlueprints();

		bool StateTrue(Zoner::GameStates state);
		bool StateFalse(Zoner::GameStates state);
		void State(Zoner::GameStates state, bool value);
	};
}