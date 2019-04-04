#include "StarSystem.h"

ZonerX::StarSystem::StarSystem()
{
	auto renderer = new ZonerX::StarSystemRenderer();
	renderer->star_system = this;
	render_object_node.AddComponent(renderer);
}

void ZonerX::StarSystem::OnDayBegin()
{
	auto update_loop = [](std::vector<ZonerX::IActor*>& list) {
		for (auto&& item : list)
		{
			item->OnDayBegin();
		}
	};

	update_loop(ship_list);
	update_loop(station_list);
	update_loop(planet_list);
	update_loop(jump_gate_list);
	update_loop(jump_hole_list);
	update_loop(meteorite_list);
	update_loop(lootbox_list);
	update_loop(star_list);
	update_loop(platform_list);
}

void ZonerX::StarSystem::OnDayEnd()
{
	auto update_loop = [](std::vector<ZonerX::IActor*>& list) {
		for (auto&& item : list)
		{
			item->OnDayEnd();
		}
	};

	update_loop(ship_list);
	update_loop(station_list);
	update_loop(planet_list);
	update_loop(jump_gate_list);
	update_loop(jump_hole_list);
	update_loop(meteorite_list);
	update_loop(lootbox_list);
	update_loop(star_list);
	update_loop(platform_list);
}

void ZonerX::StarSystem::Render(float dt)
{
	ok::graphics::Camera::PushCamera(&camera);

	
	//render background

	//render objects
	auto render_object_loop = [](std::vector<ZonerX::IActor*> list, float dt) {
		for (auto&& item : list)
		{
			item->render_object_node.Update(dt);
		}
	};

	//ok::AssetsBasic::instance().GetMaterial("Planet.Material.xml");

	sprite_batch.SetMaterial(nullptr);
	sprite_batch.BatchBegin(100);
	//sprite_batch.Draw(ok::Assets::instance().GetTexture(""), glm::vec2(0, 0), glm::vec2(200, 200));
		render_object_loop(star_list, dt);
	sprite_batch.BatchEnd();

	//ok::AssetsBasic::instance().GetMaterial("Planet.Material.xml");
	//sprite_batch.SetMaterial();
	sprite_batch.SetMaterial(ok::AssetsBasic::instance().GetMaterial("Planet.Material.xml"));
	sprite_batch.BatchBegin(101);
		render_object_loop(planet_list, dt);
	sprite_batch.BatchEnd();

	sprite_batch.SetMaterial(nullptr);
	sprite_batch.BatchBegin(102);
		render_object_loop(station_list, dt);
		render_object_loop(jump_gate_list, dt);
		render_object_loop(jump_hole_list, dt);
		render_object_loop(platform_list, dt);
	sprite_batch.BatchEnd();

	sprite_batch.BatchBegin(103);
		render_object_loop(meteorite_list, dt);
		render_object_loop(lootbox_list, dt);
		render_object_loop(ship_list, dt);
	sprite_batch.BatchEnd();


	sprite_batch.BatchBegin(800);

	//render objects ui
	auto render_ui_loop = [](std::vector<ZonerX::IActor*> list, float dt) {
		for (auto&& item : list)
		{
			item->render_ui_node.Update(dt);
		}
	};

	render_ui_loop(star_list, dt);
	render_ui_loop(planet_list, dt);
	render_ui_loop(station_list, dt);
	render_ui_loop(jump_gate_list, dt);
	render_ui_loop(jump_hole_list, dt);
	render_ui_loop(platform_list, dt);
	render_ui_loop(meteorite_list, dt);
	render_ui_loop(lootbox_list, dt);
	render_ui_loop(ship_list, dt);

	sprite_batch.BatchEnd();

	ok::graphics::Camera::PopCamera();
}

void ZonerX::StarSystemRenderer::Update(float dt)
{
	star_system->Render(dt);
}
