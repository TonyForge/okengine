#include "Planet.h"

ZonerX::Planet::Planet()
{
	auto renderer = new ZonerX::PlanetRenderer();
	render_object_node.AddComponent(renderer);
}

void ZonerX::Planet::OnDayBegin()
{
}

void ZonerX::Planet::OnDayEnd()
{
}

void ZonerX::PlanetRenderer::Update(float dt)
{
	local_time += dt*0.25f;
	if (local_time > 1.f) local_time -= 1.f;

	auto& location = *static_cast<ZonerX::IStarSystem*>(ZonerX::IGame::current_star_system);

	//location.sprite_batch.SetMaterial(ok::AssetsBasic::instance().GetMaterial("Planet.Material.xml"));

	
	ok::graphics::SpriteInfo spr_info;

	ok::graphics::TextureRect tex_rect;
	tex_rect.texture = ok::Assets::instance().GetTexture("");
	tex_rect.uv_rect = glm::vec4(0.f, 0.f, 1.f, 1.f);
	tex_rect.width = 200;
	tex_rect.height = 200;

	spr_info.rect = tex_rect;
	spr_info.hotspot = glm::vec2(0.5f, 0.5f);
	spr_info.tint_color = ok::Color(local_time,0.f, 1.f, 1.f);
	spr_info.tint_power = 1.f;

	location.sprite_batch.Draw(&spr_info, glm::vec2(transform().GetPosition()), 0.f, glm::vec2(1.f, 1.f));
	//location.sprite_batch.Draw(&tex_rect, glm::vec2(transform().GetPosition()),0.f, glm::vec2(1.f,1.f));
	//ok::graphics::SpriteInfo spr;
	//spr.rect.
	//location.sprite_batch.Draw()
}
