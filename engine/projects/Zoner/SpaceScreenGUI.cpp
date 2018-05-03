#include "SpaceScreenGUI.h"

void Zoner::SpaceScreenGUI::Update(float dt)
{
	Update_Inventory(dt);
}

void Zoner::SpaceScreenGUI::Update_Inventory(float dt)
{
	static ok::ui::widget inventory_panel_top;
	static ok::ui::widget inventory_panel_top_button_close;
	static ok::ui::widget inventory_panel_top_button_eject;

	static ok::graphics::SpriteAtlas* atlas_ui = nullptr;
	static ok::graphics::SpriteInfo spr_inventory_top_body_shadow;
	static ok::graphics::SpriteInfo spr_inventory_top_body;
	static ok::graphics::SpriteInfo spr_inventory_top_big_light;
	static ok::graphics::SpriteInfo spr_inventory_top_small_light;
	static ok::graphics::SpriteInfo spr_inventory_top_btn_close;
	static ok::graphics::SpriteInfo spr_inventory_top_btn_eject;

	if (atlas_ui == nullptr)
	{
		atlas_ui = Zoner::IGame::o().GetSpriteAtlases()["ui"];
		spr_inventory_top_body_shadow = atlas_ui->Get(ok::String("inventory_top_body_shadow"));
		spr_inventory_top_body = atlas_ui->Get(ok::String("inventory_top_body"));

		spr_inventory_top_body_shadow.scale.x = 2.f;
		spr_inventory_top_body_shadow.scale.y = 2.f;

		spr_inventory_top_big_light = atlas_ui->Get(ok::String("inventory_top_big_light"));
		spr_inventory_top_big_light.tint_color = ok::Color(255,162,0,255);
		spr_inventory_top_big_light.tint_power = 1.f;

		spr_inventory_top_small_light = atlas_ui->Get(ok::String("inventory_top_small_light"));

		spr_inventory_top_btn_close = atlas_ui->Get(ok::String("inventory_top_btn_close"));
		spr_inventory_top_btn_eject = atlas_ui->Get(ok::String("inventory_top_btn_eject"));

		spr_inventory_top_btn_close.hotspot = glm::vec2(0.f, 0.f);
		spr_inventory_top_btn_eject.hotspot = glm::vec2(0.f, 0.f);
	}

	ok::ui::BeginUI(Zoner::IGame::o().GetScreenWidth(), Zoner::IGame::o().GetScreenHeight());

	ok::ui::PushTranslate(0.f, 0.f);
	{
		ok::ui::PushTranslate(177.f, 121.f);
		{
			//shadow
			ok::ui::Image(inventory_panel_top.ptr(), &spr_inventory_top_body_shadow);

			ok::ui::DisableSmooth();
			{
				//top main
				ok::ui::Blit(inventory_panel_top.ptr(), &spr_inventory_top_body);

				//small lights
				ok::ui::Blit(inventory_panel_top.ptr(), &spr_inventory_top_small_light, -24, -72);
				ok::ui::Blit(inventory_panel_top.ptr(), &spr_inventory_top_small_light, -24 + 14, -72);
				ok::ui::Blit(inventory_panel_top.ptr(), &spr_inventory_top_small_light, -24 + 14 + 15, -72);
			}
			ok::ui::EnableSmooth();

			//big lights
			ok::ui::Image(inventory_panel_top.ptr(), &spr_inventory_top_big_light,-108,-15);
			ok::ui::Image(inventory_panel_top.ptr(), &spr_inventory_top_big_light, 56, -29);
		}
		ok::ui::PopTranslate();

		
		ok::ui::DisableSmooth();
		{
			//inventory close button
			ok::ui::PushTranslate(202, 27);
			if (ok::ui::Dummy(inventory_panel_top_button_close.ptr(), 0, 0, 39, 38).mouse_down)
			{
				if (ok::ui::ws().on_activate)
				{
					//close inventory panel action
				}
			}
			else
			{
				ok::ui::Blit(inventory_panel_top_button_close.ptr(), &spr_inventory_top_btn_close);
			}
			ok::ui::PopTranslate();

			//inventory eject button
			ok::ui::PushTranslate(23, 172);
			if (ok::ui::Dummy(inventory_panel_top_button_eject.ptr(), 0, 0, 39, 39).mouse_down)
			{
				if (ok::ui::ws().on_activate)
				{
					//close inventory panel action
				}
			}
			else
			{
				ok::ui::Blit(inventory_panel_top_button_eject.ptr(), &spr_inventory_top_btn_eject);
			}
			ok::ui::PopTranslate();
		}
		ok::ui::EnableSmooth();
		
	}
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
