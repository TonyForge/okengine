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
	enum class GameStates
	{
		PreloaderVisible = 0,
		DefaultResourcesLoaded,
		Count
	};

	class Game : public ok::Application, public Zoner::IGame
	{
	public:
		Game();
		void Init();
		void Update(float dt);
	protected:
	private:
		ok::GameObject _scene_root;
		Zoner::Preloader _preloader;

		void ShowPreloader();
		void HidePreloader();

		bool StateTrue(Zoner::GameStates state);
		bool StateFalse(Zoner::GameStates state);
		void State(Zoner::GameStates state, bool value);

		std::unordered_map<std::string, ok::GameObject*> _ship_blueprints;
		bool _game_states[static_cast<int>(Zoner::GameStates::Count)];

	public:
		virtual int GetScreenWidth();
		virtual int GetScreenHeight();
	};
}