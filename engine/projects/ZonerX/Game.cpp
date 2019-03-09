#include "Game.h"

ZonerX::Game::Game()
{
	_settings_file_name = "zonerx.settings.xml";
	instance = this;
}

void ZonerX::Game::Init()
{
	DisableFeature(ok::ApplicationFeature::AutoClearStencil);
}

void ZonerX::Game::Update(float dt)
{

}