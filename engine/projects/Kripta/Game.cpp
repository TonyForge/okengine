#include "Game.h"

Kripta::Game::Game()
{
	_settings_file_name = "kripta.settings.xml";
}

void Kripta::Game::Init()
{
	DisableFeature(ok::ApplicationFeature::AutoClearStencil);
}

void Kripta::Game::Update(float dt)
{

}