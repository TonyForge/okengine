#include "Game.h"


Zoner::Game::Game()
{
	_settings_file_name = "zoner.settings.xml";

	memset(_game_states, false, sizeof(_game_states));

	Zoner::IGame::_instance = this;
}


void Zoner::Game::Init()
{
	DisableFeature(ok::ApplicationFeature::AutoClearStencil);
}


void Zoner::Game::Update(float dt)
{
	if (StateFalse(Zoner::GameStates::DefaultResourcesLoaded) && StateFalse(Zoner::GameStates::PreloaderVisible))
	{
		_preloader.Task_DefaultResources();
		ShowPreloader();
	}

	if (StateTrue(Zoner::GameStates::DefaultResourcesLoaded) && StateTrue(Zoner::GameStates::PreloaderVisible))
	{
		HidePreloader();
	}

	if (StateTrue(Zoner::GameStates::PreloaderVisible))
	{
		_preloader.Update(dt);
	}
	else
	{
		_scene_root.Update(dt);
	}
}

void Zoner::Game::ShowPreloader()
{
	_preloader.LoadItself();
	State(Zoner::GameStates::PreloaderVisible, true);
}

void Zoner::Game::HidePreloader()
{
	State(Zoner::GameStates::PreloaderVisible, false);
	_preloader.UnloadItself();
}

bool Zoner::Game::StateTrue(Zoner::GameStates state)
{
	return _game_states[static_cast<int>(state)] == true;
}

bool Zoner::Game::StateFalse(Zoner::GameStates state)
{
	return _game_states[static_cast<int>(state)] == false;
}

void Zoner::Game::State(Zoner::GameStates state, bool value)
{
	_game_states[static_cast<int>(state)] = value;
}

int Zoner::Game::GetScreenWidth()
{
	return screen_width;
}

int Zoner::Game::GetScreenHeight()
{
	return screen_height;
}
