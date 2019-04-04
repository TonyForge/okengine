#include "Game.h"

ZonaX::Game::Game()
{
	_settings_file_name = "zonax.settings.xml";
}

void ZonaX::Game::Init()
{
	DisableFeature(ok::ApplicationFeature::AutoClearStencil);

}

void ZonaX::Game::Update(float dt)
{
}