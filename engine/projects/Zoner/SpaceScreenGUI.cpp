#include "SpaceScreenGUI.h"

void Zoner::SpaceScreenGUI::Update(float dt)
{
	Update_Inventory(dt);
}

void Zoner::SpaceScreenGUI::Update_Inventory(float dt)
{
	static ok::ui::widget inventory_panel_top;
	static ok::graphics::SpriteAtlas* atlas_ui = nullptr;
	static ok::graphics::SpriteInfo spr_inventory_top_body_shadow;
	static ok::graphics::SpriteInfo spr_inventory_top_body;

	if (atlas_ui == nullptr)
	{
		atlas_ui = Zoner::IGame::o().GetSpriteAtlases()["ui"];
		spr_inventory_top_body_shadow = atlas_ui->Get(ok::String("inventory_top_body_shadow"));
		spr_inventory_top_body = atlas_ui->Get(ok::String("inventory_top_body"));

		spr_inventory_top_body_shadow.scale.x = 2.f;
		spr_inventory_top_body_shadow.scale.y = 2.f;
	}

	ok::ui::BeginUI(Zoner::IGame::o().GetScreenWidth(), Zoner::IGame::o().GetScreenHeight());


	ok::ui::PushTranslate(177.f, 121.f);

	ok::ui::Image(inventory_panel_top.ptr(), &spr_inventory_top_body_shadow, 0, 0);

	ok::ui::DisableSmooth();
	ok::ui::BlitImage(inventory_panel_top.ptr(), &spr_inventory_top_body, 0, 0);
	ok::ui::EnableSmooth();

	ok::ui::PopTranslate();

	ok::ui::EndUI();
}

Zoner::SpaceScreenGUI & Zoner::SpaceScreenGUI::instance()
{
	static Zoner::SpaceScreenGUI s;
	return s;
}

Zoner::SpaceScreenGUI & Zoner::SpaceScreenGUI::o()
{
	return instance();
}
