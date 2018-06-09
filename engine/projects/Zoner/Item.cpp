#include "Item.h"

void Zoner::ItemCargoHold::InitBlocks(int blocks_count)
{
	_items.resize(10 * blocks_count);
	std::fill(_items.begin(), _items.end(), nullptr);

	//Zoner::ISpaceScreenGUI::o().
}

void Zoner::ItemCargoHold::PackItems()
{
	//std::fill(std::remove_if(_items.begin(), _items.end(), nullptr), _items.end(), nullptr);
	//std::partition
}

void Zoner::ItemCargoHold::SwapItems(Zoner::IItem *& item_to_swap, int x, int y)
{
	std::swap(item_to_swap, _items[x + y * 5]);
}

Zoner::IItem * Zoner::ItemCargoHold::PlaceItem(Zoner::IItem * item_to_place, int x, int y)
{
	std::swap(item_to_place, _items[x + y * 5]);

	return item_to_place;
}

Zoner::IItem * Zoner::ItemCargoHold::FindItem(int x, int y)
{
	return _items[x + y * 5];
}

Zoner::IItem * Zoner::ItemCargoHold::FindItem(int index)
{
	return _items[index];
}

Zoner::IItem * Zoner::ItemCargoHold::FindItem(ok::String & item_name)
{
	for (auto&& _item : _items)
	{
		if (_item != nullptr && _item->gameObject().name == item_name)
		{
			return _item;
		}
	}

	return nullptr;
}

std::vector<Zoner::IItem*>& Zoner::ItemCargoHold::Items()
{
	return _items;
}

void Zoner::ItemCargoHold::LoadFrom(tinyxml2::XMLDocument & doc, tinyxml2::XMLElement & element)
{
	InitBlocks(element.IntAttribute("blocks"));
}

Zoner::IItem * Zoner::ItemCargoHold::FindItem(Zoner::UID & item_uid)
{
	Zoner::IItem* result = nullptr;

	for (auto&& _item : _items)
	{
		if (_item != nullptr)
		{
			if (*_item == item_uid) return _item;
			else
			{
				result = _item->FindItem(item_uid);
				if (result != nullptr) break;
			}
		}
	}

	return result;
}

void Zoner::ItemCargoHold::CollectItems(std::map<Zoner::UID, Zoner::IItem*>& _collection)
{
	for (auto&& _item : _items)
	{
		if (_item != nullptr)
		{
			_collection[*_item] = _item;
			_item->CollectItems(_collection);
		}
	}
}

void Zoner::ItemCargoHold::CreateUI()
{
}

void Zoner::ItemCargoHold::UpdateUI(float dt)
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
			ok::ui::Image(container_widget.ptr(), &spr_back, 0.f, -32.f, -1.f, 108.f + 82.f*(container_blocks - 1));

			ok::ui::Image(container_widget.ptr(), &spr_top_side_shadow, 0.f, 0.f);
			ok::ui::Image(container_widget.ptr(), &spr_bottom_side_shadow, 0.f, 53.f + 82.f*(container_blocks - 1));

			ok::ui::DisableSmooth();
			{
				ok::ui::Image(container_widget.ptr(), &spr_left_side_shadow, -110.5f - 45.f, -0.5f, -1.f, 54.f + 82.f * (container_blocks - 1));
				ok::ui::Image(container_widget.ptr(), &spr_right_side_shadow, -110.5f + 218.5f + 45.f, -0.5f, -1.f, 54.f + 82.f * (container_blocks - 1));

				ok::ui::Blit(container_widget.ptr(), &spr_top_side, 0.f, 0.f);

				ok::ui::Blit(container_widget.ptr(), &spr_bottom_side, 0.f, 53.f + 82.f*(container_blocks - 1));

				ok::ui::Image(container_widget.ptr(), &spr_left_side, -110.5f, -0.5f, -1.f, 54.f + 82.f * (container_blocks - 1));
				ok::ui::Image(container_widget.ptr(), &spr_right_side, -110.5f + 218.5f, -0.5f, -1.f, 54.f + 82.f * (container_blocks - 1));
			}
			ok::ui::EnableSmooth();

		}
		ok::ui::PopTranslate();
		ok::ui::PopTranslate();
	}
	ok::ui::PopTranslate();
	ok::ui::PopNonActivable();
}

void Zoner::ItemCargoHold::DestroyUI()
{
}

Zoner::ItemBuilder::ItemBuilder()
{
	_instance = this;
	_behaviours_allocator["ItemSpacecraft"] = []() { return new Zoner::ItemSpacecraft(); };
	_behaviours_allocator["ItemCargoHold"] = []() { return new Zoner::ItemCargoHold(); };
}

Zoner::IItem * Zoner::ItemBuilder::BuildFromRecipe(ok::String & recipe)
{
	ok::String path = ok::Assets::instance().assets_root_folder + "items\\recipes\\" + recipe;

	tinyxml2::XMLDocument doc;
	doc.LoadFile(path.toAnsiString().c_str());

	Zoner::Item* result = new Zoner::Item();
	ok::String attrib;

	auto elem = doc.FirstChildElement("recipe")->FirstChildElement();

	while (elem != nullptr)
	{
		if (ok::String(elem->Value()) == "behaviour")
		{
			auto _behaviour = _behaviours_allocator[elem->Attribute("class")]();
			_behaviour->LoadFrom(doc, *elem);

			result->AddComponent(_behaviour);
		}
		else
		if (ok::String(elem->Value()) == "blueprint")
		{
			attrib = elem->Attribute("type");

			if (attrib == "spacecraft")
			{
				result->_blueprint_spacecraft = Zoner::IGame::o().GetShipBlueprints()[elem->Attribute("name")];
			}
			else
			{
				result->_blueprint_item = Zoner::IGame::o().GetItemBlueprints()[elem->Attribute("name")];
				//result->_blueprint_item = 
			}
		}

		elem = elem->NextSiblingElement();
	}

	return result;
}

void Zoner::ItemSpacecraft::LoadFrom(tinyxml2::XMLDocument & doc, tinyxml2::XMLElement & element)
{
	capacity = element.IntAttribute("capacity");
	capacity_left = 0;

	auto elem = element.FirstChildElement("equipment_slots_up");
	if (elem != nullptr && elem->IntAttribute("count") != 0)
	{
		int count = elem->IntAttribute("count");
	}

	elem = element.FirstChildElement("equipment_slots_middle");
	if (elem != nullptr && elem->IntAttribute("count") != 0)
	{
		int count = elem->IntAttribute("count");
	}

	elem = element.FirstChildElement("equipment_slots_down");
	if (elem != nullptr && elem->IntAttribute("count") != 0)
	{
		int count = elem->IntAttribute("count");
	}

	elem = element.FirstChildElement("container_slot");
	if (elem != nullptr)
	{
		elem = elem->FirstChildElement();
		if (elem != nullptr)
		{
			if (ok::String(elem->Value()) == "recipe")
			{
				container_slot = Zoner::IItemBuilder::o().BuildFromRecipe(ok::String(elem->Attribute("name")));
			}
		}
	}
}

Zoner::IItem * Zoner::ItemSpacecraft::FindItem(Zoner::UID & item_uid)
{
	Zoner::IItem* result = nullptr;

	//equipment slots
	for (auto&& _item : equipment_slots)
	{
		if (*_item == item_uid) return _item;
		else
		{
			result = _item->FindItem(item_uid);
			if (result != nullptr) return result;
		}
	}

	//container slot
	if (container_slot != nullptr)
	{
		if (*container_slot == item_uid) return container_slot;
		else
		{
			result = container_slot->FindItem(item_uid);
			if (result != nullptr) return result;
		}
	}

	return result;
}

void Zoner::ItemSpacecraft::CollectItems(std::map<Zoner::UID, Zoner::IItem*>& _collection)
{
	for (auto&& _item : equipment_slots)
	{
		_collection[*_item] = _item;
		_item->CollectItems(_collection);
	}

	if (container_slot != nullptr)
	{
		_collection[*container_slot] = container_slot;
		container_slot->CollectItems(_collection);
	}
}

void Zoner::ItemSpacecraft::CreateUI()
{
	_icon_cache_id = Zoner::ISpaceScreenGUI::o().ReserveIconsCache(_icon_cache_size, 1);

	if (equipment_slots_up.size() < 5)
	{
		equipment_slots_up.resize(5);
		std::fill(equipment_slots_up.begin(), equipment_slots_up.end(), nullptr);
	}

	if (equipment_slots_middle.size() < 4)
	{
		equipment_slots_middle.resize(4);
		std::fill(equipment_slots_middle.begin(), equipment_slots_middle.end(), nullptr);
	}

	if (equipment_slots_down.size() < 5)
	{
		equipment_slots_down.resize(5);
		std::fill(equipment_slots_down.begin(), equipment_slots_down.end(), nullptr);
	}
	

	//recache icons
	Zoner::IItem* _item;
	//up slots
	for (int i = 0; i < 5; i++)
	{
		if ((i + _equipment_slots_up_offset) >= static_cast<int>(equipment_slots_up.size()) || equipment_slots_up[i + _equipment_slots_up_offset] == nullptr)
		{
			//do nothing
		}
		else
		{
			//recache it
			_item = equipment_slots_up[i + _equipment_slots_up_offset];
			if (_item->_blueprint_item != nullptr)
			{
				Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_id, _item->_blueprint_item, i, 0);
			}
			else
			{
				Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_id, _item->_blueprint_spacecraft, i, 0);
			}
		}
	}
	//middle slots
	for (int i = 0; i < 4; i++)
	{
		if (equipment_slots_middle[i] == nullptr)
		{
			//do nothing
		}
		else
		{
			//recache it
			_item = equipment_slots_middle[i];
			if (_item->_blueprint_item != nullptr)
			{
				Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_id, _item->_blueprint_item, 5+i, 0);
			}
			else
			{
				Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_id, _item->_blueprint_spacecraft, 5+i, 0);
			}
		}
	}
	//down slots
	for (int i = 0; i < 5; i++)
	{
		if ((i + _equipment_slots_down_offset) >= static_cast<int>(equipment_slots_down.size()) || equipment_slots_down[i + _equipment_slots_down_offset] == nullptr)
		{
			//do nothing
		}
		else
		{
			//recache it
			_item = equipment_slots_down[i + _equipment_slots_down_offset];
			if (_item->_blueprint_item != nullptr)
			{
				Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_id, _item->_blueprint_item, 9+i, 0);
			}
			else
			{
				Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_id, _item->_blueprint_spacecraft, 9+i, 0);
			}
		}
	}
}

void Zoner::ItemSpacecraft::UpdateUI(float dt)
{
	Zoner::IItem* parent_item = static_cast<Zoner::IItem*>(&gameObject());

	static ok::ui::widget body_widget;
	static ok::ui::widget viewer_widget;
	static ok::ui::widget layout_btn_widgets[4];

	static ok::ui::widget* layout_btn_widget_activated = &layout_btn_widgets[0];
	static ok::ui::widget slot_widget;

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

	static ok::graphics::SpriteInfo spr_empty_slot_32;
	static ok::graphics::SpriteInfo spr_full_slot_32;

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

		spr_empty_slot_32 = atlas_ui->Get(ok::String("inventory_top_small_slot_empty"));
		spr_empty_slot_32.hotspot = glm::vec2(0.0, 0.0);

		spr_full_slot_32 = atlas_ui->Get(ok::String("inventory_top_small_slot"));
		spr_full_slot_32.hotspot = glm::vec2(0.0, 0.0);

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
				ok::ui::PushTranslate(131.f - 24.f, 358.f);
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

			ok::ui::Text(&text_spacecraft_type_cache, 70 + 105 - 10, 228 - 216, ok::graphics::TextAlign::Center);
			ok::ui::Text(&text_spacecraft_name_cache, 70 + 105 - 10, 228 + 36 - 216, ok::graphics::TextAlign::Center, ok::graphics::TextAlign::Center);
			ok::ui::Text(&text_spacecraft_captain_cache, 70 + 105 - 10, 228 + 65 - 216, ok::graphics::TextAlign::Center);

			//viewer
			ok::ui::PushTranslate(103.f, 177.f);
			{
				ok::ui::PushNonActivable(false);
				ok::ui::Dummy(viewer_widget.ptr(), 0.f, 0.f, 124.f, 124.f);
				ok::ui::PopNonActivable();
				
				ok::ui::Model(parent_item->_blueprint_spacecraft, glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.f, 1.f, 1.f), 124.f*0.5f, 124.f*0.5f);
			}
			ok::ui::PopTranslate();

			//up slots
			ok::ui::PushTranslate(61.f, 138.f);
			{


				for (int i = 0; i < 5; i++)
				{
					if ((i + _equipment_slots_up_offset) >= static_cast<int>(equipment_slots_up.size()) || equipment_slots_up[i + _equipment_slots_up_offset] == nullptr)
					{
						ok::ui::Image(slot_widget.ptr(), &spr_empty_slot_32, i * 43.f);
					}
					else
					{
						ok::ui::Image(slot_widget.ptr(), &spr_full_slot_32, i * 43.f);
						ok::ui::Image(slot_widget.ptr(), &Zoner::ISpaceScreenGUI::o().GetIconCache(_icon_cache_id, i, 0), i * 43.f + 2.f, 2.f);
					}
				}

				int highlighted_slot_index = -1;

				ok::ui::PushNonActivable(false);
				for (int i = 0; i < 5; i++)
				{
					ok::ui::Dummy(slot_widget.ptr(), i*43.f, 0.f, 36.f, 36.f);
					if (ok::ui::ws().mouse_inside)
					{
						highlighted_slot_index = i;
						break;
					}
				}
				ok::ui::PopNonActivable();

				if (highlighted_slot_index != -1)
				{

				}
			}
			ok::ui::PopTranslate();


			//middle slots
			{
				int highlighted_slot_index = -1;

				ok::ui::PushNonActivable(false);
				ok::ui::Dummy(slot_widget.ptr(), 60.f, 194.f, 36.f, 36.f);
				if (ok::ui::ws().mouse_inside) { highlighted_slot_index = 0; }
				else 
				{
					ok::ui::Dummy(slot_widget.ptr(), 234.f, 194.f, 36.f, 36.f);
					if (ok::ui::ws().mouse_inside) { highlighted_slot_index = 1; }
					else
					{
						ok::ui::Dummy(slot_widget.ptr(), 60.f, 248.f, 36.f, 36.f);
						if (ok::ui::ws().mouse_inside) { highlighted_slot_index = 2; }
						else
						{
							ok::ui::Dummy(slot_widget.ptr(), 234.f, 248.f, 36.f, 36.f);
							if (ok::ui::ws().mouse_inside) { highlighted_slot_index = 3; }
						}
					}
				}
				ok::ui::PopNonActivable();

				if (highlighted_slot_index != -1)
				{

				}
			}

			if (equipment_slots_middle[0] == nullptr)
			{
				ok::ui::Image(slot_widget.ptr(), &spr_empty_slot_32, 60.f, 194.f);
			}
			else
			{
				ok::ui::Image(slot_widget.ptr(), &spr_full_slot_32, 60.f, 194.f);
				ok::ui::Image(slot_widget.ptr(), &Zoner::ISpaceScreenGUI::o().GetIconCache(_icon_cache_id, 5+0, 0), 60.f + 2.f, 194.f+2.f);
			}

			if (equipment_slots_middle[1] == nullptr)
			{
				ok::ui::Image(slot_widget.ptr(), &spr_empty_slot_32, 234.f, 194.f);
			}
			else
			{
				ok::ui::Image(slot_widget.ptr(), &spr_full_slot_32, 234.f, 194.f);
				ok::ui::Image(slot_widget.ptr(), &Zoner::ISpaceScreenGUI::o().GetIconCache(_icon_cache_id, 5 + 1, 0), 234.f + 2.f, 194.f + 2.f);
			}

			if (equipment_slots_middle[2] == nullptr)
			{
				ok::ui::Image(slot_widget.ptr(), &spr_empty_slot_32, 60.f, 248.f);
			}
			else
			{
				ok::ui::Image(slot_widget.ptr(), &spr_full_slot_32, 60.f, 248.f);
				ok::ui::Image(slot_widget.ptr(), &Zoner::ISpaceScreenGUI::o().GetIconCache(_icon_cache_id, 5 + 2, 0), 60.f + 2.f, 248.f + 2.f);
			}

			if (equipment_slots_middle[3] == nullptr)
			{
				ok::ui::Image(slot_widget.ptr(), &spr_empty_slot_32, 234.f, 248.f);
			}
			else
			{
				ok::ui::Image(slot_widget.ptr(), &spr_full_slot_32, 234.f, 248.f);
				ok::ui::Image(slot_widget.ptr(), &Zoner::ISpaceScreenGUI::o().GetIconCache(_icon_cache_id, 5 + 3, 0), 234.f + 2.f, 248.f + 2.f);
			}


			//down slots
			ok::ui::PushTranslate(61.f, 306.f);
			{
				for (int i = 0; i < 5; i++)
				{
					if ((i + _equipment_slots_down_offset) >= static_cast<int>(equipment_slots_down.size()) || equipment_slots_down[i + _equipment_slots_down_offset] == nullptr)
					{
						ok::ui::Image(slot_widget.ptr(), &spr_empty_slot_32, i * 43.f);
					}
					else
					{
						ok::ui::Image(slot_widget.ptr(), &spr_full_slot_32, i * 43.f);
						ok::ui::Image(slot_widget.ptr(), &Zoner::ISpaceScreenGUI::o().GetIconCache(_icon_cache_id, 9+i, 0), i * 43.f + 2.f, 2.f);
					}
				}

				int highlighted_slot_index = -1;

				ok::ui::PushNonActivable(false);
				for (int i = 0; i < 5; i++)
				{
					ok::ui::Dummy(slot_widget.ptr(), i*43.f, 0.f, 36.f, 36.f);
					if (ok::ui::ws().mouse_inside)
					{
						highlighted_slot_index = i;
						break;
					}
				}
				ok::ui::PopNonActivable();

				if (highlighted_slot_index != -1)
				{

				}
			}
			ok::ui::PopTranslate();
		}
		ok::ui::PopTranslate();

		
	}
	ok::ui::PopTranslate();
	ok::ui::PopNonActivable();


	//ok::ui::EndUI();
}

void Zoner::ItemSpacecraft::DestroyUI()
{
	Zoner::ISpaceScreenGUI::o().ReleaseIconsCache(_icon_cache_id);
}

Zoner::IItem * Zoner::Item::FindItem(Zoner::UID & item_uid)
{
	Zoner::IItem* result = nullptr;
	Zoner::IItem* child_ptr;

	auto child_list = GetChildrens();

	for (auto&& child : child_list)
	{
		child_ptr = static_cast<Zoner::IItem*>(&(child->gameObject()));
		if (*child_ptr == item_uid) return child_ptr;
		else
		{
			result = child_ptr->FindItem(item_uid);
			if (result != nullptr) return result;
		}
	}

	result = nullptr;

	auto components = GetComponents();
	for (auto&& component : components)
	{
		result = static_cast<Zoner::IItemBehaviour*>(component)->FindItem(item_uid);
		if (result != nullptr) break;
	}

	return result;
}

void Zoner::Item::CollectItems(std::map<Zoner::UID, Zoner::IItem*>& _collection)
{
	Zoner::IItem* child_ptr;

	auto child_list = GetChildrens();

	for (auto&& child : child_list)
	{
		child_ptr = static_cast<Zoner::IItem*>(&(child->gameObject()));

		_collection[*child_ptr] = child_ptr;

		child_ptr->CollectItems(_collection);
	}

	auto components = GetComponents();
	for (auto&& component : components)
	{
		static_cast<Zoner::IItemBehaviour*>(component)->CollectItems(_collection);
	}
}

void Zoner::Item::CreateUI()
{
	auto components = GetComponents();
	for (auto&& component : components)
	{
		static_cast<Zoner::IItemBehaviour*>(component)->CreateUI();
	}
}

void Zoner::Item::UpdateUI(float dt)
{
	auto components = GetComponents();
	for (auto&& component : components)
	{
		static_cast<Zoner::IItemBehaviour*>(component)->UpdateUI(dt);
	}
}

void Zoner::Item::DestroyUI()
{
	auto components = GetComponents();
	for (auto&& component : components)
	{
		static_cast<Zoner::IItemBehaviour*>(component)->DestroyUI();
	}
}
