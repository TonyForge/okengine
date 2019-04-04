#include "Game.h"

ZonerX::Game::Game()
{
	_settings_file_name = "zonerx.settings.xml";
	instance = this;
}

void ZonerX::Game::Init()
{
	DisableFeature(ok::ApplicationFeature::AutoClearStencil);

	auto star_system = new ZonerX::StarSystem();
	current_star_system = star_system;

	auto planet = new ZonerX::Planet();
	star_system->planet_list.push_back(planet);
}

void ZonerX::Game::Update(float dt)
{
	if (current_star_system != nullptr)
	{
		current_star_system->render_object_node.Update(dt);
	}
}