#include "SpaceScreenGUI.h"

void Zoner::SpaceScreenGUI::Update(float dt)
{
	if (o()._initialized == false)
	{
		o()._inspector_items.reserve(32);
		o()._inspector_items.resize(5);
		std::fill(o()._inspector_items.begin(), o()._inspector_items.end(), 0);

		o()._inspector_items_in_slots.resize(5);
		std::fill(o()._inspector_items_in_slots.begin(), o()._inspector_items_in_slots.end(), nullptr);

		o()._initialized = true;
	}

	if (o()._icons_cache_64px == nullptr)
	{
		o()._icons_cache_64px = new ok::graphics::RenderTarget(512, 512, true, true, false, true);
		o()._icons_cache_64px_tex = new ok::graphics::Texture(o()._icons_cache_64px);
	}

	if (Zoner::IGame::o().StateTrue(Zoner::GameStates::InspectorSwitch))
	{
		if (Zoner::IGame::o().StateFalse(Zoner::GameStates::InspectorVisible))
		{
			Create_Inspector();
			Zoner::IGame::o().State(Zoner::GameStates::InspectorVisible, true);
		}
	}

	if (Zoner::IGame::o().StateFalse(Zoner::GameStates::InspectorSwitch))
	{
		if (Zoner::IGame::o().StateTrue(Zoner::GameStates::InspectorVisible))
		{
			Destroy_Inspector();
			Zoner::IGame::o().State(Zoner::GameStates::InspectorVisible, false);
		}
	}

	ok::Input::o().SetCurrentLayer(1);
	ok::ui::BeginUI(Zoner::IGame::o().GetScreenWidth(), Zoner::IGame::o().GetScreenHeight());
	{
		if (Zoner::IGame::o().StateTrue(Zoner::GameStates::InspectorVisible))
		{
			Update_Inspector(dt);
			Update_Item_Spacecraft(dt);
			Update_Item_Container(dt);
		}
	}
	ok::ui::EndUI();
	ok::Input::o().SetCurrentLayer(0);
}

void Zoner::SpaceScreenGUI::Update_Inspector(float dt)
{
	
	

	static ok::ui::widget inventory_panel_top;
	static ok::ui::widget inventory_panel_top_button_close;
	static ok::ui::widget inventory_panel_top_button_eject;
	static ok::ui::widget inventory_panel_top_small_slot;
	static ok::ui::widget inventory_panel_top_big_slot;

	struct inventory_panel_top_scroll_struct
	{
		ok::ui::widget widget;
		int items_visible_first_index;
		float scroll_button_relative_position;
		float scroll_button_relative_size;
	} static inventory_panel_top_scroll;


	static ok::graphics::SpriteAtlas* atlas_ui = nullptr;
	static ok::graphics::SpriteInfo spr_inventory_top_body_shadow;
	static ok::graphics::SpriteInfo spr_inventory_top_body;
	static ok::graphics::SpriteInfo spr_inventory_top_big_light;
	static ok::graphics::SpriteInfo spr_inventory_top_small_light;
	static ok::graphics::SpriteInfo spr_inventory_top_btn_close;
	static ok::graphics::SpriteInfo spr_inventory_top_btn_eject;

	static ok::graphics::SpriteInfo spr_inventory_top_scroll_back;
	static ok::graphics::SpriteInfo spr_inventory_top_knob_l;
	static ok::graphics::SpriteInfo spr_inventory_top_knob_m;
	static ok::graphics::SpriteInfo spr_inventory_top_knob_r;

	static ok::graphics::SpriteInfo spr_inventory_top_knob_glow_l;
	static ok::graphics::SpriteInfo spr_inventory_top_knob_glow_m;
	static ok::graphics::SpriteInfo spr_inventory_top_knob_glow_r;

	static ok::graphics::SpriteInfo spr_empty_slot_32;
	static ok::graphics::SpriteInfo spr_full_slot_32;

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

		spr_inventory_top_scroll_back = atlas_ui->Get(ok::String("inventory_top_scroll_back"));
		spr_inventory_top_knob_l = atlas_ui->Get(ok::String("inventory_top_knob_l"));
		spr_inventory_top_knob_m = atlas_ui->Get(ok::String("inventory_top_knob_m"));
		spr_inventory_top_knob_r = atlas_ui->Get(ok::String("inventory_top_knob_r"));
		spr_inventory_top_knob_glow_l = atlas_ui->Get(ok::String("inventory_top_knob_glow_l"));
		spr_inventory_top_knob_glow_m = atlas_ui->Get(ok::String("inventory_top_knob_glow_m"));
		spr_inventory_top_knob_glow_r = atlas_ui->Get(ok::String("inventory_top_knob_glow_r"));

		spr_inventory_top_scroll_back.hotspot = glm::vec2(0.f, 0.f);
		spr_inventory_top_knob_l.hotspot = glm::vec2(0.f, 0.f);
		spr_inventory_top_knob_m.hotspot = glm::vec2(0.f, 0.f);
		spr_inventory_top_knob_r.hotspot = glm::vec2(0.f, 0.f);

		spr_inventory_top_knob_glow_l.hotspot = glm::vec2(1.f, 0.5f);
		spr_inventory_top_knob_glow_m.hotspot = glm::vec2(0.f, 0.5f);
		spr_inventory_top_knob_glow_r.hotspot = glm::vec2(0.f, 0.5f);

		inventory_panel_top_scroll.items_visible_first_index = 0;
		inventory_panel_top_scroll.scroll_button_relative_position = 0.f;
		inventory_panel_top_scroll.scroll_button_relative_size = 0.f;

		spr_empty_slot_32 = atlas_ui->Get(ok::String("inventory_top_small_slot_empty"));
		spr_empty_slot_32.hotspot = glm::vec2(0.0, 0.0);

		spr_full_slot_32 = atlas_ui->Get(ok::String("inventory_top_small_slot"));
		spr_full_slot_32.hotspot = glm::vec2(0.0, 0.0);

		ok::Input::o().AddBlockedArea(0, ok::Rect2Di(114, 38, 107, 92));
		ok::Input::o().AddBlockedArea(0, ok::Rect2Di(202, 26, 40, 40));
		ok::Input::o().AddBlockedArea(0, ok::Rect2Di(22, 172, 40, 40));
		ok::Input::o().AddBlockedArea(0, ok::Rect2Di(54, 119, 232, 88));
		ok::Input::o().AddBlockedArea(0, ok::Rect2Di(35, 129, 266, 49));
	}

	//ok::ui::BeginUI(Zoner::IGame::o().GetScreenWidth(), Zoner::IGame::o().GetScreenHeight());
	ok::ui::PushNonActivable(true);
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
			ok::ui::PushNonActivable(false);
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
			ok::ui::PopNonActivable();
			

			//scroll
			ok::ui::PushTranslate(67, 184);
			{
				ok::ui::PushTranslate(1, 1);
				{
					ok::ui::PushNonActivable(false);
					{
						int prev_items_index = inventory_panel_top_scroll.items_visible_first_index;

						ok::ui::ScrollHorizontal(
							inventory_panel_top_scroll.widget.ptr(),
							0, 0, 197, 10,
							5, o()._inspector_items.size(),
							inventory_panel_top_scroll.items_visible_first_index,
							inventory_panel_top_scroll.scroll_button_relative_position,
							inventory_panel_top_scroll.scroll_button_relative_size
						);

						if (o()._inspector_recache_icons == true || 
							prev_items_index != inventory_panel_top_scroll.items_visible_first_index)
						{
							auto player_ship = Zoner::IGame::o().GetCurrentPlayerShip();
							auto& player_items = player_ship->this_items;
							
							//update small slots
							for (int i = 0; i < 5; i++)
							{
								if (o()._inspector_items[inventory_panel_top_scroll.items_visible_first_index + i] == 0)
								{
									o()._inspector_items_in_slots[i] = nullptr;
								}
								else
								{
									auto _item_in_slot = player_items[o()._inspector_items[inventory_panel_top_scroll.items_visible_first_index + i]];

									if (_item_in_slot->_blueprint_item != nullptr)
									{
										o()._CacheIcon(_item_in_slot->_blueprint_item, i, 0);
									}
									else if (_item_in_slot->_blueprint_spacecraft != nullptr)
									{
										o()._CacheIcon(_item_in_slot->_blueprint_spacecraft, i, 0);
									}

									o()._inspector_items_in_slots[i] = _item_in_slot;
								}
							}

							o()._inspector_recache_icons = false;
						}
					}
					ok::ui::PopNonActivable();

					//ok::ui::PushNonActivable(true);
					//{
						//knob glow begin
						ok::ui::Blit(
							inventory_panel_top_scroll.widget.ptr(),
							&spr_inventory_top_knob_glow_l,
							inventory_panel_top_scroll.scroll_button_relative_position +
								spr_inventory_top_knob_l.rect.width,
							5
						);

						ok::ui::Blit(
							inventory_panel_top_scroll.widget.ptr(),
							&spr_inventory_top_knob_glow_r,
							inventory_panel_top_scroll.scroll_button_relative_position + 
								glm::floor(inventory_panel_top_scroll.scroll_button_relative_size) - 
								spr_inventory_top_knob_r.rect.width,
							5
						);

						ok::ui::Image(
							inventory_panel_top_scroll.widget.ptr(),
							&spr_inventory_top_knob_glow_m,
							glm::floor(inventory_panel_top_scroll.scroll_button_relative_position) + spr_inventory_top_knob_l.rect.width,
							5,
							inventory_panel_top_scroll.scroll_button_relative_size - spr_inventory_top_knob_l.rect.width - spr_inventory_top_knob_r.rect.width - 0.5f
						);
						//knob glow end

						//scroll background
						ok::ui::Blit(inventory_panel_top_scroll.widget.ptr(), &spr_inventory_top_scroll_back, -1, -1);

						//knob begin
						ok::ui::Blit(
							inventory_panel_top_scroll.widget.ptr(),
							&spr_inventory_top_knob_l,
							inventory_panel_top_scroll.scroll_button_relative_position,
							1
						);

						ok::ui::Blit(
							inventory_panel_top_scroll.widget.ptr(),
							&spr_inventory_top_knob_r,
							inventory_panel_top_scroll.scroll_button_relative_position + glm::floor(inventory_panel_top_scroll.scroll_button_relative_size) - spr_inventory_top_knob_r.rect.width,
							1
						);

						ok::ui::Image(
							inventory_panel_top_scroll.widget.ptr(),
							&spr_inventory_top_knob_m,
							glm::floor(inventory_panel_top_scroll.scroll_button_relative_position) + spr_inventory_top_knob_l.rect.width - 1,
							1,
							inventory_panel_top_scroll.scroll_button_relative_size - spr_inventory_top_knob_l.rect.width - spr_inventory_top_knob_r.rect.width + 2
						);
						//knob end
					//}
					//ok::ui::PopNonActivable();
				}
				
				ok::ui::PopTranslate();
			}
			ok::ui::PopTranslate();
		}
		ok::ui::EnableSmooth();
		
		//small slots
		ok::ui::PushTranslate(65.f, 135.f);
		{
			if (o()._drag_and_drop_item == nullptr)
			{
				//if there is NO item in hand
				int highlighted_slot_index = -1;

				ok::ui::PushNonActivable(false);
				for (int i = 0; i < 5; i++)
				{
					ok::ui::Dummy(inventory_panel_top_small_slot.ptr(), i * 42.f, 0.f, 36.f, 36.f);

					if (ok::ui::ws().mouse_inside)
					{
						highlighted_slot_index = i;
						break;
					}
				}
				ok::ui::PopNonActivable();

				if (ok::ui::ws().mouse_inside)
				{
					if (ok::ui::ws().on_activate)
					{
						if (o()._inspector_items_in_slots[highlighted_slot_index] != nullptr)
						{
							//pick up item
							o()._drag_and_drop_item = o()._inspector_items_in_slots[highlighted_slot_index];

							//we gain shortcut if pick up from inspector
							o()._drag_and_drop_item_shortcut = &(o()._inspector_items[inventory_panel_top_scroll.items_visible_first_index + highlighted_slot_index]);
						}	
					}
					else if (ok::ui::ws().mouse_down == false)
					{
						if (o()._inspector_items_in_slots[highlighted_slot_index] != nullptr)
						{
							//draw tip to place this item in big slot
							//request item info bubble drawing
						}
					}
				}
			}
			else
			{
				//if there is item in hand
				int highlighted_slot_index = -1;

				ok::ui::PushNonActivable(false);
				for (int i = 0; i < 5; i++)
				{
					ok::ui::Dummy(inventory_panel_top_small_slot.ptr(), i * 42.f, 0.f, 36.f, 36.f);

					if (ok::ui::ws().mouse_inside)
					{
						highlighted_slot_index = i;
						break;
					}
				}
				ok::ui::PopNonActivable();

				if (ok::ui::ws().mouse_inside)
				{
					if (ok::ui::ws().on_activate)
					{
						if (o()._inspector_items_in_slots[highlighted_slot_index] == nullptr)
						{
							//place item in empty slot
							o()._inspector_items[inventory_panel_top_scroll.items_visible_first_index + highlighted_slot_index] = *o()._drag_and_drop_item;

							if (o()._drag_and_drop_item_shortcut != nullptr)
							{
								*(o()._drag_and_drop_item_shortcut) = 0;
							}

							o()._drag_and_drop_item = nullptr;
							o()._drag_and_drop_item_shortcut = nullptr;
						}
						else
						{
							// if it's the same place where item was
							if (&(o()._inspector_items[inventory_panel_top_scroll.items_visible_first_index + highlighted_slot_index]) == o()._drag_and_drop_item_shortcut)
							{
								//just release item
								o()._drag_and_drop_item = nullptr;
								o()._drag_and_drop_item_shortcut = nullptr;
							}
							else
							{
								//to do: there will be some special logic for placing item over containers for example (so we put item inside instead of exchange picked items)
								//for now just exchange items...

								//EXCHANGE LOGIC
								{
									//place item in non empty slot and pick up new item
									o()._inspector_items[inventory_panel_top_scroll.items_visible_first_index + highlighted_slot_index] = *o()._drag_and_drop_item;

									//we loose shortcut here (if was), but pick up new item,
									//so we can create new shortcut if place it in inspector, or not otherwise...
									if (o()._drag_and_drop_item_shortcut != nullptr)
									{
										*(o()._drag_and_drop_item_shortcut) = 0;
										o()._drag_and_drop_item_shortcut = nullptr;
									}


									o()._drag_and_drop_item = o()._inspector_items_in_slots[highlighted_slot_index];
								}
							}
						}

						o()._inspector_recache_icons = true;
					}
				}
			}

			

			for (int i = 0; i < 5; i++)
			{
				if (o()._inspector_items_in_slots[i] == nullptr)
				{
					ok::ui::Image(inventory_panel_top.ptr(), &spr_empty_slot_32, i * 42.f);
				}
				else
				{
					ok::ui::Image(inventory_panel_top.ptr(), &spr_full_slot_32, i * 42.f);

					if (o()._inspector_items_in_slots[i] == o()._drag_and_drop_item)
					{
						ok::ui::PushEffect_Fade(0.5f);
						{
							ok::ui::Image(inventory_panel_top.ptr(), &o()._GetIconCache(i, 0), i * 42.f + 2.f, 2.f);
						}
						ok::ui::PopEffect_Fade();
					}
					else
					{
						ok::ui::Image(inventory_panel_top.ptr(), &o()._GetIconCache(i, 0), i * 42.f + 2.f, 2.f);
					}
					
				}
			}
		}
		ok::ui::PopTranslate();

		//big slot
		ok::ui::PushTranslate(136.f, 59.f);
		{
			ok::ui::PushNonActivable(false);
			ok::ui::Dummy(inventory_panel_top_big_slot.ptr(), 0.f, 0.f, 63.f, 63.f);
			ok::ui::PopNonActivable();

			if (o()._drag_and_drop_item == nullptr)
			{
				if (ok::ui::ws().mouse_inside)
				{
					if (ok::ui::ws().on_activate)
					{
						if (o()._inspector_big_slot_item != nullptr)
						{
							o()._inspector_big_slot_item = nullptr;

							//Close interface for this item
						}
					}
				}
			}
			else
			{
				if (ok::ui::ws().mouse_inside)
				{
					if (ok::ui::ws().on_activate)
					{
						if (o()._inspector_big_slot_item == nullptr)
						{
							//place item in empty slot
							o()._inspector_big_slot_item = o()._drag_and_drop_item;

							o()._drag_and_drop_item = nullptr;
							o()._drag_and_drop_item_shortcut = nullptr;

							//Open interface for this item
						}
					}
				}
			}
			
			if (o()._inspector_big_slot_item == nullptr)
			{
				//do nothing
			}
			else
			{
				ok::GameObject* _blueprint = nullptr;

				if (o()._inspector_big_slot_item->_blueprint_item != nullptr)
				{
					_blueprint = o()._inspector_big_slot_item->_blueprint_item;
				}
				else if (o()._inspector_big_slot_item->_blueprint_spacecraft != nullptr)
				{
					_blueprint = o()._inspector_big_slot_item->_blueprint_spacecraft;
				}

				if (_blueprint != nullptr)
				{
					ok::ui::FlushBatch();
					//scissor is broken (fullscreen multisampling done via 2x resolution and scissor doesnt know about this)
					//somehow scissor come from right down corner in screen mode O_o
					ok::ui::PushCrop(0.f, 0.f, 63.f, 63.f);
					ok::ui::Model(_blueprint, glm::vec3(0.f, o().rot1, 0.f), glm::vec3(90.f - 30.f, -41.5f, -20.18f), glm::vec3(2.f, 2.f, 2.f), 63.f / 2.f, 63.f / 2.f);
					ok::ui::PopCrop();
				}				
			}

			//ok::graphics::RenderTarget::CopyColorBetween(*o()._icons_cache_64px, *o()._item_snapshot, 0, 0, 0, 0, 1, 1);
		}
		ok::ui::PopTranslate();

	}
	ok::ui::PopTranslate();

	ok::ui::PopNonActivable();

	//inspectors changes here
	o().rot1 += dt*30.f;
	//extend inspector small slots
	if (o()._inspector_items_in_slots[4] != nullptr &&
		inventory_panel_top_scroll.items_visible_first_index + 4 == o()._inspector_items.size() - 1)
	{
		o()._inspector_items.push_back(Zoner::UID());
	}

	//shrink inspector small slots
	while (
		o()._inspector_items.size() > 5 &&
		o()._inspector_items[o()._inspector_items.size() - 1] == 0 &&
		o()._inspector_items[o()._inspector_items.size() - 2] == 0)
	{
		o()._inspector_items.pop_back();
	}
}

void Zoner::SpaceScreenGUI::Update_Item_Spacecraft(float dt)
{
	static ok::ui::widget body_widget;
	static ok::ui::widget layout_btn_widgets[4];

	static ok::ui::widget* layout_btn_widget_activated = &layout_btn_widgets[0];

	static ok::graphics::SpriteAtlas* atlas_ui = nullptr;

	static ok::graphics::SpriteInfo spr_body;
	static ok::graphics::SpriteInfo spr_body_shadow;
	static ok::graphics::SpriteInfo spr_big_light;

	static ok::graphics::SpriteInfo spr_btn_layout_up;
	static ok::graphics::SpriteInfo spr_btn_layout_down;
	static ok::graphics::SpriteInfo spr_btn_layout_light;

	static ok::String text_spacecraft_type = L"Òðàíñïîðòíûé êîðàáëü";
	static ok::String text_spacecraft_name = L"ÊÎÐÎËÅÂÀ ÇÈÎÍÀ";
	static ok::String text_spacecraft_captain = L"Êàïèòàí Êèðê";

	static ok::graphics::TextCache text_spacecraft_type_cache;
	static ok::graphics::TextCache text_spacecraft_name_cache;
	static ok::graphics::TextCache text_spacecraft_captain_cache;

	if (atlas_ui == nullptr)
	{
		atlas_ui = Zoner::IGame::o().GetSpriteAtlases()["ui"];

		spr_body = atlas_ui->Get(ok::String("inventory_ship_body"));
		spr_body_shadow = atlas_ui->Get(ok::String("inventory_ship_body_shadow"));

		spr_body_shadow.scale.x = 2.f;
		spr_body_shadow.scale.y = 2.f;

		spr_body.hotspot.x = 0.f;
		spr_body.hotspot.y = 0.f;

		spr_body_shadow.hotspot.x = 0.f;
		spr_body_shadow.hotspot.y = 0.f;

		spr_big_light = atlas_ui->Get(ok::String("inventory_top_big_light"));
		spr_big_light.tint_color = ok::Color(255, 162, 0, 255);
		spr_big_light.tint_power = 1.f;

		spr_btn_layout_up = atlas_ui->Get(ok::String("inventory_ship_layout_btn"));
		spr_btn_layout_down = atlas_ui->Get(ok::String("inventory_ship_layout_btn_down"));

		spr_btn_layout_light = atlas_ui->Get(ok::String("inventory_top_big_light"));
		spr_btn_layout_light.tint_color = ok::Color(255, 162, 0, 255);
		spr_btn_layout_light.tint_power = 1.f;

		ok::Input::o().AddBlockedArea(0, ok::Rect2Di(52, 216, 245, 497));
		ok::Input::o().AddBlockedArea(0, ok::Rect2Di(19, 239, 314, 157));
		ok::Input::o().AddBlockedArea(0, ok::Rect2Di(11, 440, 45, 88));
		ok::Input::o().AddBlockedArea(0, ok::Rect2Di(285, 416, 81, 77));
		ok::Input::o().AddBlockedArea(0, ok::Rect2Di(21, 560, 310, 61));
		ok::Input::o().AddBlockedArea(0, ok::Rect2Di(31, 616, 288, 90));
		//ok::graphics::Font* 

		//Poka cechiruem text zdes, potom sravnivat texti s tekushim corablem i cechirovat esli otlichayutsa
		ok::graphics::TextBatch2D& text_batch = ok::ui::GetTextBatch();

		text_batch.CacheBegin();
		{
			text_batch.SetBrushFont(ok::Assets::instance().GetFont("lt_steel_b_xl_01"));
			text_batch.Draw(text_spacecraft_type);
		}
		text_spacecraft_type_cache = *text_batch.CacheEnd();

		text_batch.SetBrushPosition(0.f, 0.f);

		text_batch.CacheBegin();
		{
			//text_batch.SetBrushFont(ok::Assets::instance().GetFont("lt_steel_b_xl_01"));
			text_batch.Draw(text_spacecraft_captain);
		}
		text_spacecraft_captain_cache = *text_batch.CacheEnd();

		text_batch.SetBrushPosition(0.f, 0.f);

		text_batch.CacheBegin();
		{
			text_batch.SetBrushFont(ok::Assets::instance().GetFont("plat_b_xxl_01"));
			text_batch.Draw(text_spacecraft_name);
		}
		text_spacecraft_name_cache = *text_batch.CacheEnd();
	}

	//ok::ui::BeginUI(Zoner::IGame::o().GetScreenWidth(), Zoner::IGame::o().GetScreenHeight());
	
	ok::ui::PushNonActivable(true);
	ok::ui::PushTranslate(0.f, 0.f);
	{
		ok::ui::PushTranslate(10.f, 216.f);
		{
			ok::ui::PushTranslate(-51.f, -50.f);
			{
				//shadow
				ok::ui::Image(body_widget.ptr(), &spr_body_shadow);
			}
			ok::ui::PopTranslate();

			ok::ui::DisableSmooth();
			{
				//top main
				ok::ui::Blit(body_widget.ptr(), &spr_body);

				//layout buttons
				ok::ui::PushTranslate(131.f-24.f, 358.f);
				{
					ok::ui::PushNonActivable(false);
					{
						for (auto& layout_btn : layout_btn_widgets)
						{
							ok::ui::PushTranslate(24.f, 0.f);
							if (ok::ui::Dummy(layout_btn.ptr(), -10, -10, 21, 21).mouse_down)
							{
								if (ok::ui::ws().on_activate)
								{
									layout_btn_widget_activated = layout_btn.ptr();
								}
							}
						}
					}
					ok::ui::PopNonActivable();

					ok::ui::PopTranslate();
					ok::ui::PopTranslate();
					ok::ui::PopTranslate();
					ok::ui::PopTranslate();

					for (auto& layout_btn : layout_btn_widgets)
					{
						ok::ui::PushTranslate(24.f, 0.f);
						if (layout_btn_widget_activated == layout_btn.ptr())
						{
							ok::ui::Blit(layout_btn.ptr(), &spr_btn_layout_down);
						}
						else
						{
							ok::ui::Blit(layout_btn.ptr(), &spr_btn_layout_up);
						}
					}

					ok::ui::PopTranslate();
					ok::ui::PopTranslate();
					ok::ui::PopTranslate();
					ok::ui::PopTranslate();
				}

				ok::ui::PopTranslate();
			}
			ok::ui::EnableSmooth();

			ok::ui::Image(body_widget.ptr(), &spr_big_light, 318.f, 463.f);

			ok::ui::Text(&text_spacecraft_type_cache, 70 + 105-10, 228-216, ok::graphics::TextAlign::Center);
			ok::ui::Text(&text_spacecraft_name_cache, 70 + 105-10, 228 + 36-216, ok::graphics::TextAlign::Center, ok::graphics::TextAlign::Center);
			ok::ui::Text(&text_spacecraft_captain_cache, 70 + 105-10, 228 + 65-216, ok::graphics::TextAlign::Center);
		}
		ok::ui::PopTranslate();
	}
	ok::ui::PopTranslate();
	ok::ui::PopNonActivable();

	
	//ok::ui::EndUI();
}

void Zoner::SpaceScreenGUI::Update_Item_Container(float dt)
{
	static ok::ui::widget container_widget;

	static ok::graphics::SpriteAtlas* atlas_ui = nullptr;

	static ok::graphics::SpriteInfo spr_back;

	static ok::graphics::SpriteInfo spr_right_side;
	static ok::graphics::SpriteInfo spr_left_side;
	static ok::graphics::SpriteInfo spr_top_side;
	static ok::graphics::SpriteInfo spr_bottom_side;

	static ok::graphics::SpriteInfo spr_right_side_shadow;
	static ok::graphics::SpriteInfo spr_left_side_shadow;
	static ok::graphics::SpriteInfo spr_top_side_shadow;
	static ok::graphics::SpriteInfo spr_bottom_side_shadow;

	if (atlas_ui == nullptr)
	{
		atlas_ui = Zoner::IGame::o().GetSpriteAtlases()["ui"];

		spr_back = atlas_ui->Get(ok::String("inventory_container_back"));

		spr_right_side = atlas_ui->Get(ok::String("inventory_container_right_side"));
		spr_left_side = atlas_ui->Get(ok::String("inventory_container_left_side"));
		spr_top_side = atlas_ui->Get(ok::String("inventory_container_top_side"));
		spr_bottom_side = atlas_ui->Get(ok::String("inventory_container_bottom_side"));

		spr_right_side_shadow = atlas_ui->Get(ok::String("inventory_container_right_side_shadow"));
		spr_left_side_shadow = atlas_ui->Get(ok::String("inventory_container_left_side_shadow"));
		spr_top_side_shadow = atlas_ui->Get(ok::String("inventory_container_top_side_shadow"));
		spr_bottom_side_shadow = atlas_ui->Get(ok::String("inventory_container_bottom_side_shadow"));

		spr_right_side_shadow.scale.x = 2.f;
		//spr_right_side_shadow.scale.y = 2.f;

		spr_left_side_shadow.scale.x = 2.f;
		//spr_left_side_shadow.scale.y = 2.f;

		spr_top_side_shadow.scale.x = 2.f;
		spr_top_side_shadow.scale.y = 2.f;

		spr_bottom_side_shadow.scale.x = 2.f;
		spr_bottom_side_shadow.scale.y = 2.f;


		spr_back.hotspot.x = 0.5f;
		spr_back.hotspot.y = 0.f;

		spr_right_side.hotspot.x = 0.5f;
		spr_right_side.hotspot.y = 0.f;

		spr_left_side.hotspot.x = 0.5f;
		spr_left_side.hotspot.y = 0.f;

		spr_top_side.hotspot.x = 0.5f;
		spr_top_side.hotspot.y = 1.f;

		spr_bottom_side.hotspot.x = 0.5f;
		spr_bottom_side.hotspot.y = 0.f;


		spr_top_side_shadow.hotspot.x = 0.5f;
		spr_top_side_shadow.hotspot.y = 1.f;

		spr_bottom_side_shadow.hotspot.x = 0.5f;
		spr_bottom_side_shadow.hotspot.y = 0.f;

		spr_right_side_shadow.hotspot.x = 0.5f;
		spr_right_side_shadow.hotspot.y = 0.0f;

		spr_left_side_shadow.hotspot.x = 0.5f;
		spr_left_side_shadow.hotspot.y = 0.0f;
	}

	int container_blocks = 4;

	ok::ui::PushNonActivable(true);
	ok::ui::PushTranslate(0.f, 0.f);
	{
		ok::ui::PushTranslate(356.f, 291.f);
		ok::ui::PushTranslate(293.f / 2.f, 58.f);
		{
			ok::ui::Image(container_widget.ptr(), &spr_back, 0.f, -32.f,-1.f,108.f+82.f*(container_blocks-1));

			ok::ui::Image(container_widget.ptr(), &spr_top_side_shadow, 0.f, 0.f);
			ok::ui::Image(container_widget.ptr(), &spr_bottom_side_shadow, 0.f, 53.f + 82.f*(container_blocks - 1));

			ok::ui::DisableSmooth();
			{
				ok::ui::Image(container_widget.ptr(), &spr_left_side_shadow, -110.5f - 45.f, -0.5f, -1.f, 54.f + 82.f * (container_blocks - 1));
				ok::ui::Image(container_widget.ptr(), &spr_right_side_shadow, -110.5f + 218.5f + 45.f, -0.5f, -1.f, 54.f + 82.f * (container_blocks - 1));

				ok::ui::Blit(container_widget.ptr(), &spr_top_side, 0.f, 0.f);

				ok::ui::Blit(container_widget.ptr(), &spr_bottom_side, 0.f, 53.f+82.f*(container_blocks-1));

				ok::ui::Image(container_widget.ptr(), &spr_left_side, -110.5f, -0.5f, -1.f, 54.f + 82.f * (container_blocks-1));
				ok::ui::Image(container_widget.ptr(), &spr_right_side, -110.5f+218.5f, -0.5f, -1.f, 54.f + 82.f * (container_blocks-1));
			}
			ok::ui::EnableSmooth();
	
		}
		ok::ui::PopTranslate();
		ok::ui::PopTranslate();
	}
	ok::ui::PopTranslate();
	ok::ui::PopNonActivable();
}

void Zoner::SpaceScreenGUI::Create_Inspector()
{
	auto player_ship = Zoner::IGame::o().GetCurrentPlayerShip();
	auto& player_items = player_ship->this_items;

	//recheck all items that was in inspector before
	for (auto& item_uid : o()._inspector_items)
	{
		if (item_uid == 0)
		{
			//do nothing
		}
		else
		{
			if (player_items.find(item_uid) == player_items.end()) //no such item in players inventory anymore
			{
				//remove it from small slot
				for (auto& item_in_slot : o()._inspector_items_in_slots)
				{
					if ((item_in_slot != nullptr) && (*item_in_slot == item_uid))
					{
						item_in_slot = nullptr;
						break;
					}
				}

				//remove it from big slot
				if ((o()._inspector_big_slot_item != nullptr) && (*o()._inspector_big_slot_item == item_uid))
				{
					o()._inspector_big_slot_item = nullptr;
				}

				//remove it from inspector
				item_uid = 0;
			}
		}
	}

	//first item is always ship hull
	o()._inspector_items[0] = *(player_ship->this_item);
	/*o()._inspector_items.push_back(*(player_ship->this_item));
	o()._inspector_items.push_back(*(player_ship->this_item));
	o()._inspector_items.push_back(*(player_ship->this_item));
	o()._inspector_items.push_back(*(player_ship->this_item));
	o()._inspector_items.push_back(*(player_ship->this_item));
	o()._inspector_items.push_back(*(player_ship->this_item));*/

	o()._inspector_recache_icons = true;
}

void Zoner::SpaceScreenGUI::Destroy_Inspector()
{
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

void Zoner::SpaceScreenGUI::_CacheIcon(ok::GameObject * blueprint, int slot_x, int slot_y)
{
	const glm::vec3 blueprint_rotation(90.f - 30.f, -41.5f, -20.18f);

	_icons_cache_64px->BindTarget();

	ok::graphics::Camera camera(ok::graphics::CameraCoordinateSystem::Cartesian);
	camera.SetProjectionOrtho(512.f, 512.f, 1.f, 1000.f);

	camera.BeginTransform();
	camera.SetPosition(glm::vec3(0.f, 0.f, -500.0f));
	camera.EndTransform(false);

	ok::Transform _container;

	_container.AddChild(blueprint);

	_container.BeginTransform();
	_container.SetRotation(blueprint_rotation);
	_container.EndTransform(true);

	glm::vec4 bounds;
	_CalculateBlueprintBounds(blueprint, bounds);

	bounds.x -= 2.f;
	bounds.y -= 2.f;
	bounds.z += 2.f;
	bounds.w += 2.f;

	glm::vec3 bounder_center((bounds.x + bounds.z)*0.5f, (bounds.y + bounds.w)*0.5f, 0.f);

	float scale = 64.f / glm::max(bounds.z - bounds.x, bounds.w - bounds.y);


	_container.BeginTransform();
		_container.SetScale(glm::vec3(1.f, 1.f, 1.f) * scale);
		_container.SetPosition(-bounder_center*scale+glm::vec3(static_cast<float>(slot_x) * 64.f + 32.f, static_cast<float>(slot_y) * 64.f + 32.f, 0.f));
	_container.EndTransform(true);

	ok::graphics::Camera::PushCamera(&camera);
	camera.BeginScissorTest(slot_x * 64, slot_y * 64, 64, 64);

	glClearColor(0.f, 0.f, 0.f, 0.f);
	glDepthMask(GL_TRUE);
	glClearDepth(0.f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ok::graphics::LayeredRenderer::instance().BeginImmediateRender();
	blueprint->Update(0.f);
	ok::graphics::LayeredRenderer::instance().EndImmediateRender();

	camera.EndScissorTest();
	ok::graphics::Camera::PopCamera();

	//_container.AddChild(blueprint);

	_container.BeginTransform();
	_container.SetScale(glm::vec3(1.f, 1.f, 1.f));
	_container.SetPosition(glm::vec3(0.f, 0.f, 0.f));
	_container.SetRotation(glm::vec3(0.f, 0.f, 0.f));
	_container.EndTransform(true);

	_container.RemoveChild(blueprint);

	_icons_cache_64px->UnbindTarget();
}

void Zoner::SpaceScreenGUI::_CalculateBlueprintBounds(ok::Transform * part, glm::vec4 & bounds)
{
	ok::graphics::MeshRenderer* mr = part->gameObject().GetComponent<ok::graphics::MeshRenderer>();

	if (mr != nullptr)
	{
		GLBufferInfo buf_info = mr->GetMesh()->GetVertices();

		struct f3
		{
			float x, y, z;
		};

		f3* positions = (f3*)(buf_info.vertices);

		glm::vec4 part_axis4;
		glm::vec3 part_axis;

		part_axis4.w = 1.f; //if = 0 then translation wont work, so we use 1.f because we need translation

		for (int i = 0; i < buf_info.vertex_count; i++)
		{
			part_axis4.x = positions->x;
			part_axis4.y = positions->y;
			part_axis4.z = positions->z;

			part_axis = glm::vec3(part->GetAbsoluteTransformMatrix() * part_axis4);

			bounds.x = glm::min(bounds.x, glm::dot(part_axis, glm::vec3(1.f, 0.f, 0.f)));
			bounds.y = glm::min(bounds.y, glm::dot(part_axis, glm::vec3(0.f, 1.f, 0.f)));

			bounds.z = glm::max(bounds.z, glm::dot(part_axis, glm::vec3(1.f, 0.f, 0.f)));
			bounds.w = glm::max(bounds.w, glm::dot(part_axis, glm::vec3(0.f, 1.f, 0.f)));

			positions++;
		}
	}
	else
	{
		glm::vec3 part_axis;

		part->BeginTransform(ok::TransformSpace::WorldSpace);
		part_axis = part->GetPosition();
		part->EndTransform(false);

		bounds.x = glm::min(bounds.x, glm::dot(part_axis, glm::vec3(1.f, 0.f, 0.f)));
		bounds.y = glm::min(bounds.y, glm::dot(part_axis, glm::vec3(0.f, 1.f, 0.f)));

		bounds.z = glm::max(bounds.z, glm::dot(part_axis, glm::vec3(1.f, 0.f, 0.f)));
		bounds.w = glm::max(bounds.w, glm::dot(part_axis, glm::vec3(0.f, 1.f, 0.f)));
	}

	std::list<ok::Transform*>& childrens = part->GetChildrens();
	for (auto& child : childrens)
	{
		_CalculateBlueprintBounds(child, bounds);
	}
}

ok::graphics::SpriteInfo Zoner::SpaceScreenGUI::_GetIconCache(int slot_x, int slot_y)
{
	ok::graphics::SpriteInfo sprite;

	sprite.rect = ok::graphics::TextureRect(_icons_cache_64px_tex, slot_x * 64, slot_y * 64, 64, 64);
	sprite.hotspot.x = 0.f;
	sprite.hotspot.y = 0.f;
	
	sprite.scale.x = 0.5f;
	sprite.scale.y = 0.5f;

	return sprite;
}
