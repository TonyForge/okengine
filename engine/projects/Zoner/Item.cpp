#include "Item.h"

void Zoner::ItemCargoHold::InitBlocks(int blocks_count)
{
	_items.resize(10 * blocks_count);
	std::fill(_items.begin(), _items.end(), nullptr);

	_icon_cache_size = _items.size();

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

	tinyxml2::XMLElement* elem;
	tinyxml2::XMLElement* inner_elem;
	tinyxml2::XMLElement* slot_elem;

	elem = element.FirstChildElement("slots");

	if (elem == nullptr)
	{
		//do nothing, no slots info for this container provided
	}
	else
	{
		for (inner_elem = elem->FirstChildElement("slot"); inner_elem != nullptr; inner_elem = inner_elem->NextSiblingElement("slot"))
		{
			slot_elem = inner_elem->FirstChildElement();
			if (slot_elem != nullptr)
			{
				if (ok::String(slot_elem->Value()) == "recipe")
				{
					//build from recipe
					Zoner::IItem* _item = Zoner::IItemBuilder::o().BuildFromRecipe(ok::String(slot_elem->Attribute("name")));

					if (ItemInAllowed(_item))
					{
						ItemIn(_item);

						_items[inner_elem->IntAttribute("x") + inner_elem->IntAttribute("y") * 5] = _item;
					}
					else
					{
						delete _item;
					}
				}
			}
		}
	}
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
	_icon_cache_id = Zoner::ISpaceScreenGUI::o().ReserveIconsCache(_icon_cache_size, 1);

	int _item_index = 0;

	for (auto&& _item : _items)
	{
		if (_item != nullptr)
		{
			if (_item->_blueprint_item != nullptr)
			{
				Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_id, _item->_blueprint_item, _item_index, 0);
			}
			else if (_item->_blueprint_spacecraft != nullptr)
			{
				Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_id, _item->_blueprint_spacecraft, _item_index, 0);
			}
		}

		_item_index++;
	}
}

void Zoner::ItemCargoHold::UpdateUI(float dt)
{
	static ok::ui::widget container_widget;
	static ok::ui::widget slot_widget;

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

	static ok::graphics::SpriteInfo spr_empty_slot_32;
	static ok::graphics::SpriteInfo spr_full_slot_32;

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

		spr_empty_slot_32 = atlas_ui->Get(ok::String("inventory_top_small_slot_empty"));
		spr_empty_slot_32.hotspot = glm::vec2(0.0, 0.0);

		spr_full_slot_32 = atlas_ui->Get(ok::String("inventory_top_small_slot"));
		spr_full_slot_32.hotspot = glm::vec2(0.0, 0.0);
	}

	int container_blocks = _items.size() / 10;

	ok::ui::PushNonActivable(true);
	ok::ui::PushTranslate(0.f, 0.f);
	{
		//ok::ui::PushTranslate(356.f, 291.f);
		ok::ui::PushTranslate(27.f, 215.f);
		{
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

			ok::ui::PushTranslate(45.f, 46.f);
			{
				int highlighted_slot_x = -1;
				int highlighted_slot_y = -1;

				ok::ui::PushNonActivable(false);
				for (int y = 0; y < _icon_cache_size / 5; y++)
				{
					for (int x = 0; x < 5; x++)
					{
						ok::ui::Dummy(slot_widget.ptr(), x * 41.f, y * 41.f, 36.f, 36.f);
						if (ok::ui::ws().mouse_inside)
						{
							highlighted_slot_x = x;
							break;
						}
					}
					if (ok::ui::ws().mouse_inside)
					{
						highlighted_slot_y = y;
						break;
					}
				}
				ok::ui::PopNonActivable();

				if (highlighted_slot_x != -1 && highlighted_slot_y != -1)
				{
					if (ok::ui::ws().on_activate)
					{
						Zoner::IItem* _drag_and_drop_item = Zoner::ISpaceScreenGUI::o().GetDragAndDropItem();

						if (_drag_and_drop_item != nullptr)
						{
							if (_items[highlighted_slot_x + highlighted_slot_y * 5] == nullptr)
							{
								//place item in empty slot
								//if both sides allowed this operation
								if (ItemInAllowed(_drag_and_drop_item) &&
									(_drag_and_drop_item->this_item_owner == nullptr || _drag_and_drop_item->this_item_owner->ItemOutAllowed(_drag_and_drop_item)))
								{
									if (_drag_and_drop_item->this_item_owner != nullptr)
									{
										_drag_and_drop_item->this_item_owner->ItemOut(_drag_and_drop_item);
									}

									ItemIn(_drag_and_drop_item);

									_items[highlighted_slot_x + highlighted_slot_y * 5] = _drag_and_drop_item;

									//cache icon
									if (_drag_and_drop_item->_blueprint_item != nullptr)
										Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_id, _drag_and_drop_item->_blueprint_item, highlighted_slot_x + highlighted_slot_y * 5, 0);
									else if (_drag_and_drop_item->_blueprint_spacecraft != nullptr)
										Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_id, _drag_and_drop_item->_blueprint_spacecraft, highlighted_slot_x + highlighted_slot_y * 5, 0);

									if (Zoner::ISpaceScreenGUI::o().IsDragAndDropItemFromInspector())
									{
										Zoner::ISpaceScreenGUI::o().RemoveDragAndDropItemFromInspector();
									}

									Zoner::ISpaceScreenGUI::o().SetDragAndDropItem(nullptr);
								}
								else
								{
									//cant move item, show effect
								}
							}
							else
							{
								//if its the same item just release it and cancel drag and drop
								if (_items[highlighted_slot_x + highlighted_slot_y * 5] == _drag_and_drop_item)
								{
									//Place container back to its slot
									//if item from inspector
									if (Zoner::ISpaceScreenGUI::o().IsDragAndDropItemFromInspector())
									{
										Zoner::ISpaceScreenGUI::o().RemoveDragAndDropItemFromInspector();
									}

									Zoner::ISpaceScreenGUI::o().SetDragAndDropItem(nullptr);
								}
								else
								{
									//place item in full slot
									//if both sides allowed this operation (exchange requires both sides to exist!)
									if (ItemInAllowed(_drag_and_drop_item, true, _items[highlighted_slot_x + highlighted_slot_y * 5]) &&
										(_drag_and_drop_item->this_item_owner != nullptr &&
											_drag_and_drop_item->this_item_owner->ItemOutAllowed(_drag_and_drop_item, true, _items[highlighted_slot_x + highlighted_slot_y * 5])))
									{
										_drag_and_drop_item->this_item_owner->ItemOut(_drag_and_drop_item, true, _items[highlighted_slot_x + highlighted_slot_y * 5]);
										ItemIn(_drag_and_drop_item, true, _items[highlighted_slot_x + highlighted_slot_y * 5]);

										Zoner::IItem* exchanged_item = _items[highlighted_slot_x + highlighted_slot_y * 5];
										_items[highlighted_slot_x + highlighted_slot_y * 5] = _drag_and_drop_item;

										//cache icon
										if (_drag_and_drop_item->_blueprint_item != nullptr)
											Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_id, _drag_and_drop_item->_blueprint_item, highlighted_slot_x + highlighted_slot_y * 5, 0);
										else if (_drag_and_drop_item->_blueprint_spacecraft != nullptr)
											Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_id, _drag_and_drop_item->_blueprint_spacecraft, highlighted_slot_x + highlighted_slot_y * 5, 0);


										if (Zoner::ISpaceScreenGUI::o().IsDragAndDropItemFromInspector())
										{
											Zoner::ISpaceScreenGUI::o().RemoveDragAndDropItemFromInspector();
										}

										//pickup exchanged item for drag and drop
										Zoner::ISpaceScreenGUI::o().SetDragAndDropItem(exchanged_item);
									}
									else
									{
										//cant move item, show effect
									}
								}
								
							}
						}
						else
						{
							//pickup item from container
							if (_items[highlighted_slot_x + highlighted_slot_y * 5] == nullptr)
							{
								//this slot is empty, do nothing
							}
							else
							{
								//slot is full, pickup item
								Zoner::ISpaceScreenGUI::o().SetDragAndDropItem(_items[highlighted_slot_x + highlighted_slot_y * 5]);
							}
						}
					}
				}

				for (int y = 0; y < _icon_cache_size / 5; y++)
				{
					for (int x = 0; x < 5; x++)
					{
						if (_items[x + y * 5] == nullptr)
						{
							ok::ui::Image(slot_widget.ptr(), &spr_empty_slot_32, x * 41.f, y * 41.f);
						}
						else
						{
							ok::ui::Image(slot_widget.ptr(), &spr_full_slot_32, x * 41.f, y * 41.f);

							if (Zoner::ISpaceScreenGUI::o().GetDragAndDropItem() == _items[x + y * 5])
							{
								ok::ui::PushEffect_Fade(0.5f);
							}

							ok::ui::Image(slot_widget.ptr(), &Zoner::ISpaceScreenGUI::o().GetIconCache(_icon_cache_id, x + y * 5, 0), x * 41.f + 2.f, y * 41.f + 2.f);

							if (Zoner::ISpaceScreenGUI::o().GetDragAndDropItem() == _items[x + y * 5])
							{
								ok::ui::PopEffect_Fade();
							}
						}
					}
				}

			}
			ok::ui::PopTranslate();
		}
		ok::ui::PopTranslate();
	}
	ok::ui::PopTranslate();
	ok::ui::PopNonActivable();
}

void Zoner::ItemCargoHold::DestroyUI()
{
	Zoner::ISpaceScreenGUI::o().ReleaseIconsCache(_icon_cache_id);
}

void Zoner::ItemCargoHold::ItemIn(Zoner::IItem * item, bool exchange, const Zoner::IItem * exchange_item)
{
	item->this_item_owner = this;
}

void Zoner::ItemCargoHold::ItemOut(const Zoner::IItem * item, bool exchange, Zoner::IItem * exchange_item)
{
	int position = std::find(_items.begin(), _items.end(), item) - _items.begin();
	if (exchange)
	{
		_items[position] = exchange_item;
		exchange_item->this_item_owner = this;

		if (exchange_item->_blueprint_item != nullptr)
			Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_id, exchange_item->_blueprint_item, position, 0);
		else if (exchange_item->_blueprint_spacecraft != nullptr)
			Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_id, exchange_item->_blueprint_spacecraft, position, 0);
	}
	else
	{
		_items[position] = nullptr;
	}
}

bool Zoner::ItemCargoHold::ItemInAllowed(Zoner::IItem * item, bool exchange, Zoner::IItem * exchange_item)
{
	auto _item = static_cast<Zoner::IItem*>(&gameObject());
	Zoner::UID& _item_uid = *_item;

	if (_item_uid == *item || item->FindItem(*_item) != nullptr)
	{
		//self nesting cycle detected, cant allow this
		return false;
	}

	if (exchange)
	{
	}
	else
	{
		bool is_space_enough = (std::find(_items.begin(), _items.end(), nullptr) != _items.end());

		if (is_space_enough == false)
		{
			return false;
		}
	}

	return true;
}

bool Zoner::ItemCargoHold::ItemOutAllowed(Zoner::IItem * item, bool exchange, Zoner::IItem * exchange_item)
{
	return true;
}

/*void Zoner::ItemCargoHold::ItemIn(Zoner::IItem * item)
{
	item->this_item_owner = this;
}

void Zoner::ItemCargoHold::ItemOut(Zoner::IItem * item)
{
	auto _item_ptr = std::find(_items.begin(), _items.end(), item);

	if (_item_ptr != _items.end())
	{
		*_item_ptr = nullptr;
	}

	item->this_item_owner = nullptr;
}*/

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
			result->AddComponent(_behaviour);

			_behaviour->LoadFrom(doc, *elem);
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
				Zoner::IItem* _item = Zoner::IItemBuilder::o().BuildFromRecipe(ok::String(elem->Attribute("name")));

				if (ItemInAllowed(_item))
				{
					ItemIn(_item);
					container_slot = _item;
				}
				else
				{
					delete _item;
				}
			}
		}
	}
}

Zoner::IItem * Zoner::ItemSpacecraft::FindItem(Zoner::UID & item_uid)
{
	Zoner::IItem* result = nullptr;

	//equipment slots
	for (auto&& _item : equipment_items)
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
	for (auto&& _item : equipment_items)
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
	_viewer_dirty = true;

	//_icon_cache_id = Zoner::ISpaceScreenGUI::o().ReserveIconsCache(_icon_cache_size, 1);
	_icon_cache_slots_up_id = Zoner::ISpaceScreenGUI::o().ReserveIconsCache(5, 1);
	_icon_cache_slots_down_id = Zoner::ISpaceScreenGUI::o().ReserveIconsCache(5, 1);
	_icon_cache_slots_middle_id = Zoner::ISpaceScreenGUI::o().ReserveIconsCache(4, 1);
	_icon_cache_container_slot_id = Zoner::ISpaceScreenGUI::o().ReserveIconsCache(1, 1);

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
				Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_slots_up_id, _item->_blueprint_item, i, 0);
			}
			else if (_item->_blueprint_spacecraft != nullptr)
			{
				Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_slots_up_id, _item->_blueprint_spacecraft, i, 0);
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
				Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_slots_middle_id, _item->_blueprint_item, i, 0);
			}
			else if (_item->_blueprint_spacecraft != nullptr)
			{
				Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_slots_middle_id, _item->_blueprint_spacecraft, i, 0);
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
				Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_slots_down_id, _item->_blueprint_item, i, 0);
			}
			else if (_item->_blueprint_spacecraft != nullptr)
			{
				Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_slots_down_id, _item->_blueprint_spacecraft, i, 0);
			}
		}
	}

	if (container_slot != nullptr)
	{
		_item = container_slot;
		if (_item->_blueprint_item != nullptr)
		{
			Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_container_slot_id, _item->_blueprint_item, 0, 0);
		}
		else if (_item->_blueprint_spacecraft != nullptr)
		{
			Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_container_slot_id, _item->_blueprint_spacecraft, 0, 0);
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

	static ok::ui::widget offset_button_widget_left_up;
	static ok::ui::widget offset_button_widget_right_up;
	static ok::ui::widget offset_button_widget_left_down;
	static ok::ui::widget offset_button_widget_right_down;

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

	static ok::graphics::SpriteInfo spr_offset_btn_left;
	static ok::graphics::SpriteInfo spr_offset_btn_left_down;
	static ok::graphics::SpriteInfo spr_offset_btn_right;
	static ok::graphics::SpriteInfo spr_offset_btn_right_down;

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

		spr_offset_btn_left = atlas_ui->Get(ok::String("button_01_small_left"));
		spr_offset_btn_left.hotspot = glm::vec2(0.0, 0.0);

		spr_offset_btn_left_down = atlas_ui->Get(ok::String("button_01_small_left_down"));
		spr_offset_btn_left_down.hotspot = glm::vec2(0.0, 0.0);

		spr_offset_btn_right = atlas_ui->Get(ok::String("button_01_small_right"));
		spr_offset_btn_right.hotspot = glm::vec2(0.0, 0.0);

		spr_offset_btn_right_down = atlas_ui->Get(ok::String("button_01_small_right_down"));
		spr_offset_btn_right_down.hotspot = glm::vec2(0.0, 0.0);

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
				ok::ui::PushTranslate(131.f - 24.f, 361.f);
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
				if (_viewer_dirty)
				{
					Zoner::ISpaceScreenGUI::o().GetRenderCache256().BindTarget();

					glClearColor(0.f, 0.f, 0.f, 0.f);
					glDepthMask(GL_TRUE);
					glClearDepth(0.f);

					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

					ok::graphics::Camera cam(ok::graphics::CameraCoordinateSystem::CartesianCenter);
					cam.SetProjectionOrtho(256, 256,1.f, 1000.f);

					cam.BeginTransform();
					cam.SetPosition(glm::vec3(0.f, 0.f, -500.f));
					cam.EndTransform(false);

					ok::graphics::Camera::PushCamera(&cam);
					ok::ui::PushResetTransform();
					ok::ui::Model(parent_item->_blueprint_spacecraft, glm::vec3(90.f, 0.f, -30.f), glm::vec3(-30.f, -70.f, -10.f), glm::vec3(3.5f, 3.5f, 3.5f));
					ok::ui::PopResetTransform();
					ok::graphics::Camera::PopCamera();

					Zoner::ISpaceScreenGUI::o().GetRenderCache256().UnbindTarget();

					_viewer_dirty = false;
				}

				ok::graphics::SpriteInfo viewer_sprite;
				viewer_sprite.rect = ok::graphics::TextureRect(&Zoner::ISpaceScreenGUI::o().GetRenderCache256Tex(), 0, 0, 256, 256);
				viewer_sprite.hotspot = glm::vec2(0.5f, 0.5f);
				viewer_sprite.scale.x = 0.5f;
				viewer_sprite.scale.y = 0.5f;

				ok::ui::Image(viewer_widget.ptr(), &viewer_sprite, 124.f*0.5f + 5.f, 124.f*0.5f);

				//ok::ui::Model(parent_item->_blueprint_spacecraft, glm::vec3(90.f, 0.f, -30.f), glm::vec3(-30.f, -70.f, -10.f), glm::vec3(1.60f, 1.60f, 1.60f), 124.f*0.5f, 124.f*0.5f);

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

						if (Zoner::ISpaceScreenGUI::o().GetDragAndDropItem() == equipment_slots_up[i + _equipment_slots_up_offset])
						{
							ok::ui::PushEffect_Fade(0.5f);
						}

						ok::ui::Image(slot_widget.ptr(), &Zoner::ISpaceScreenGUI::o().GetIconCache(_icon_cache_slots_up_id, i, 0), i * 43.f + 2.f, 2.f);

						if (Zoner::ISpaceScreenGUI::o().GetDragAndDropItem() == equipment_slots_up[i + _equipment_slots_up_offset])
						{
							ok::ui::PopEffect_Fade();
						}
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
					if (ok::ui::ws().on_activate)
					{
						Zoner::IItem* _drag_and_drop_item = Zoner::ISpaceScreenGUI::o().GetDragAndDropItem();
						Zoner::IItem* _highlighted_slot_item = equipment_slots_up[highlighted_slot_index + _equipment_slots_up_offset];

						if (_drag_and_drop_item != nullptr)
						{
							//placement logic
							if (_highlighted_slot_item == nullptr)
							{
								//place in empty slot
								//if both sides allowed this operation
								if (ItemInAllowed(_drag_and_drop_item) &&
									(_drag_and_drop_item->this_item_owner == nullptr || _drag_and_drop_item->this_item_owner->ItemOutAllowed(_drag_and_drop_item)))
								{
									if (_drag_and_drop_item->this_item_owner != nullptr)
									{
										_drag_and_drop_item->this_item_owner->ItemOut(_drag_and_drop_item);
									}

									ItemIn(_drag_and_drop_item);

									equipment_slots_up[highlighted_slot_index + _equipment_slots_up_offset] = _drag_and_drop_item;

									//cache icon
									if (_drag_and_drop_item->_blueprint_item != nullptr)
										Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_slots_up_id, _drag_and_drop_item->_blueprint_item, highlighted_slot_index, 0);
									else if (_drag_and_drop_item->_blueprint_spacecraft != nullptr)
										Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_slots_up_id, _drag_and_drop_item->_blueprint_spacecraft, highlighted_slot_index, 0);

									if (Zoner::ISpaceScreenGUI::o().IsDragAndDropItemFromInspector())
									{
										Zoner::ISpaceScreenGUI::o().RemoveDragAndDropItemFromInspector();
									}

									Zoner::ISpaceScreenGUI::o().SetDragAndDropItem(nullptr);
								}
								else
								{
									//cant move item, show effect
								}
							}
							else
							{
								//place item in full slot
								if (_highlighted_slot_item == _drag_and_drop_item)
								{
									//Place container back to its slot
									//if item from inspector
									if (Zoner::ISpaceScreenGUI::o().IsDragAndDropItemFromInspector())
									{
										Zoner::ISpaceScreenGUI::o().RemoveDragAndDropItemFromInspector();
									}

									Zoner::ISpaceScreenGUI::o().SetDragAndDropItem(nullptr);
								}
								else
								{
									//if both sides allowed this operation (exchange requires both sides to exist!)
									if (ItemInAllowed(_drag_and_drop_item, true, _highlighted_slot_item) &&
										(_drag_and_drop_item->this_item_owner != nullptr &&
											_drag_and_drop_item->this_item_owner->ItemOutAllowed(_drag_and_drop_item, true, _highlighted_slot_item)))
									{
										_drag_and_drop_item->this_item_owner->ItemOut(_drag_and_drop_item, true, _highlighted_slot_item);
										ItemIn(_drag_and_drop_item, true, _highlighted_slot_item);

										Zoner::IItem* exchanged_item = _highlighted_slot_item;
										equipment_slots_up[highlighted_slot_index + _equipment_slots_up_offset] = _drag_and_drop_item;

										//cache icon
										if (_drag_and_drop_item->_blueprint_item != nullptr)
											Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_slots_up_id, _drag_and_drop_item->_blueprint_item, highlighted_slot_index, 0);
										else if (_drag_and_drop_item->_blueprint_spacecraft != nullptr)
											Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_slots_up_id, _drag_and_drop_item->_blueprint_spacecraft, highlighted_slot_index, 0);


										if (Zoner::ISpaceScreenGUI::o().IsDragAndDropItemFromInspector())
										{
											Zoner::ISpaceScreenGUI::o().RemoveDragAndDropItemFromInspector();
										}

										//pickup exchanged item for drag and drop
										Zoner::ISpaceScreenGUI::o().SetDragAndDropItem(exchanged_item);
									}
									else
									{
										//cant move item, show effect
									}
								}
							}
						}
						else
						{
							//pickup logic
							if (_highlighted_slot_item == nullptr)
							{
								//this slot is empty, do nothing
							}
							else
							{
								//slot is full, pickup item
								Zoner::ISpaceScreenGUI::o().SetDragAndDropItem(_highlighted_slot_item);
							}
						}
					}
				}
			}
			ok::ui::PopTranslate();


			//up slots left right offset buttons
			//left
			if (_equipment_slots_up_offset > 0)
			{
				ok::ui::PushTranslate(41.f, 139.f);
				{
					ok::ui::PushNonActivable(false);
					ok::ui::Dummy(offset_button_widget_left_up.ptr(), 0, 0, 20, 35);
					ok::ui::PopNonActivable();

					if (ok::ui::ws().mouse_down)
					{
						if (ok::ui::ws().on_activate)
						{
							//act
							_equipment_slots_up_offset--;
							if (_equipment_slots_up_offset < 0) _equipment_slots_up_offset = 0;
							else
							{
								Zoner::ISpaceScreenGUI::o().MoveIconsInsideCache(_icon_cache_slots_up_id, 1, 0);

								if (equipment_slots_up[_equipment_slots_up_offset] != nullptr)
								{
									if (equipment_slots_up[_equipment_slots_up_offset]->_blueprint_item)
									{
										Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_slots_up_id, equipment_slots_up[_equipment_slots_up_offset]->_blueprint_item, 0, 0);
									}
									else if (equipment_slots_up[_equipment_slots_up_offset]->_blueprint_spacecraft)
									{
										Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_slots_up_id, equipment_slots_up[_equipment_slots_up_offset]->_blueprint_spacecraft, 0, 0);
									}
								}

							}
						}
						ok::ui::Image(offset_button_widget_left_up.ptr(), &spr_offset_btn_left_down);
					}
					else
					{
						ok::ui::Image(offset_button_widget_left_up.ptr(), &spr_offset_btn_left);
					}
				}
				ok::ui::PopTranslate();
			}
			else
			{
				ok::ui::PushTranslate(41.f, 139.f);
				{
					ok::ui::PushEffect_Fade(0.75f);
					ok::ui::Image(offset_button_widget_left_up.ptr(), &spr_offset_btn_left);
					ok::ui::PopEffect_Fade();
				}
				ok::ui::PopTranslate();
			}
			
			if (_equipment_slots_up_offset + 4 < static_cast<int>(equipment_slots_up.size())-1)
			{
				//right
				ok::ui::PushTranslate(268.f, 139.f);
				{
					ok::ui::PushNonActivable(false);
					ok::ui::Dummy(offset_button_widget_right_up.ptr(), 0, 0, 20, 35);
					ok::ui::PopNonActivable();

					if (ok::ui::ws().mouse_down)
					{
						if (ok::ui::ws().on_activate)
						{
							//act
							_equipment_slots_up_offset++;
							if (_equipment_slots_up_offset + 4 >= static_cast<int>(equipment_slots_up.size())) _equipment_slots_up_offset--;
							else
							{
								Zoner::ISpaceScreenGUI::o().MoveIconsInsideCache(_icon_cache_slots_up_id, -1, 0);

								if (equipment_slots_up[_equipment_slots_up_offset + 4] != nullptr)
								{
									if (equipment_slots_up[_equipment_slots_up_offset + 4]->_blueprint_item)
									{
										Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_slots_up_id, equipment_slots_up[_equipment_slots_up_offset + 4]->_blueprint_item, 4, 0);
									}
									else if (equipment_slots_up[_equipment_slots_up_offset + 4]->_blueprint_spacecraft)
									{
										Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_slots_up_id, equipment_slots_up[_equipment_slots_up_offset + 4]->_blueprint_spacecraft, 4, 0);
									}
								}
							}
						}
						ok::ui::Image(offset_button_widget_right_up.ptr(), &spr_offset_btn_right_down);
					}
					else
					{
						ok::ui::Image(offset_button_widget_right_up.ptr(), &spr_offset_btn_right);
					}
				}
				ok::ui::PopTranslate();
			}
			else
			{
				ok::ui::PushTranslate(268.f, 139.f);
				{
					ok::ui::PushEffect_Fade(0.75f);
					ok::ui::Image(offset_button_widget_right_up.ptr(), &spr_offset_btn_right);
					ok::ui::PopEffect_Fade();
				}
				ok::ui::PopTranslate();
			}

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
					if (ok::ui::ws().on_activate)
					{
						Zoner::IItem* _drag_and_drop_item = Zoner::ISpaceScreenGUI::o().GetDragAndDropItem();
						Zoner::IItem* _highlighted_slot_item = equipment_slots_middle[highlighted_slot_index];

						if (_drag_and_drop_item != nullptr)
						{
							//placement logic
							if (_highlighted_slot_item == nullptr)
							{
								//place in empty slot
								//if both sides allowed this operation
								if (ItemInAllowed(_drag_and_drop_item) &&
									(_drag_and_drop_item->this_item_owner == nullptr || _drag_and_drop_item->this_item_owner->ItemOutAllowed(_drag_and_drop_item)))
								{
									if (_drag_and_drop_item->this_item_owner != nullptr)
									{
										_drag_and_drop_item->this_item_owner->ItemOut(_drag_and_drop_item);
									}

									ItemIn(_drag_and_drop_item);

									equipment_slots_middle[highlighted_slot_index] = _drag_and_drop_item;

									//cache icon
									if (_drag_and_drop_item->_blueprint_item != nullptr)
										Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_slots_middle_id, _drag_and_drop_item->_blueprint_item, highlighted_slot_index, 0);
									else if (_drag_and_drop_item->_blueprint_spacecraft != nullptr)
										Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_slots_middle_id, _drag_and_drop_item->_blueprint_spacecraft, highlighted_slot_index, 0);

									if (Zoner::ISpaceScreenGUI::o().IsDragAndDropItemFromInspector())
									{
										Zoner::ISpaceScreenGUI::o().RemoveDragAndDropItemFromInspector();
									}

									Zoner::ISpaceScreenGUI::o().SetDragAndDropItem(nullptr);
								}
								else
								{
									//cant move item, show effect
								}
							}
							else
							{
								//place item in full slot
								if (_highlighted_slot_item == _drag_and_drop_item)
								{
									//Place container back to its slot
									//if item from inspector
									if (Zoner::ISpaceScreenGUI::o().IsDragAndDropItemFromInspector())
									{
										Zoner::ISpaceScreenGUI::o().RemoveDragAndDropItemFromInspector();
									}

									Zoner::ISpaceScreenGUI::o().SetDragAndDropItem(nullptr);
								}
								else
								{
									//if both sides allowed this operation (exchange requires both sides to exist!)
									if (ItemInAllowed(_drag_and_drop_item, true, _highlighted_slot_item) &&
										(_drag_and_drop_item->this_item_owner != nullptr &&
											_drag_and_drop_item->this_item_owner->ItemOutAllowed(_drag_and_drop_item, true, _highlighted_slot_item)))
									{
										_drag_and_drop_item->this_item_owner->ItemOut(_drag_and_drop_item, true, _highlighted_slot_item);
										ItemIn(_drag_and_drop_item, true, _highlighted_slot_item);

										Zoner::IItem* exchanged_item = _highlighted_slot_item;
										equipment_slots_middle[highlighted_slot_index] = _drag_and_drop_item;

										//cache icon
										if (_drag_and_drop_item->_blueprint_item != nullptr)
											Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_slots_middle_id, _drag_and_drop_item->_blueprint_item, highlighted_slot_index, 0);
										else if (_drag_and_drop_item->_blueprint_spacecraft != nullptr)
											Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_slots_middle_id, _drag_and_drop_item->_blueprint_spacecraft, highlighted_slot_index, 0);


										if (Zoner::ISpaceScreenGUI::o().IsDragAndDropItemFromInspector())
										{
											Zoner::ISpaceScreenGUI::o().RemoveDragAndDropItemFromInspector();
										}

										//pickup exchanged item for drag and drop
										Zoner::ISpaceScreenGUI::o().SetDragAndDropItem(exchanged_item);
									}
									else
									{
										//cant move item, show effect
									}
								}
							}
						}
						else
						{
							//pickup logic
							if (_highlighted_slot_item == nullptr)
							{
								//this slot is empty, do nothing
							}
							else
							{
								//slot is full, pickup item
								Zoner::ISpaceScreenGUI::o().SetDragAndDropItem(_highlighted_slot_item);
							}
						}
					}
				}
			}

			if (equipment_slots_middle[0] == nullptr)
			{
				ok::ui::Image(slot_widget.ptr(), &spr_empty_slot_32, 60.f, 194.f);
			}
			else
			{
				ok::ui::Image(slot_widget.ptr(), &spr_full_slot_32, 60.f, 194.f);

				if (Zoner::ISpaceScreenGUI::o().GetDragAndDropItem() == equipment_slots_middle[0])
				{
					ok::ui::PushEffect_Fade(0.5f);
				}

				ok::ui::Image(slot_widget.ptr(), &Zoner::ISpaceScreenGUI::o().GetIconCache(_icon_cache_slots_middle_id, 0, 0), 60.f + 2.f, 194.f + 2.f);

				if (Zoner::ISpaceScreenGUI::o().GetDragAndDropItem() == equipment_slots_middle[0])
				{
					ok::ui::PopEffect_Fade();
				}
			}

			if (equipment_slots_middle[1] == nullptr)
			{
				ok::ui::Image(slot_widget.ptr(), &spr_empty_slot_32, 234.f, 194.f);
			}
			else
			{
				ok::ui::Image(slot_widget.ptr(), &spr_full_slot_32, 234.f, 194.f);

				if (Zoner::ISpaceScreenGUI::o().GetDragAndDropItem() == equipment_slots_middle[1])
				{
					ok::ui::PushEffect_Fade(0.5f);
				}

				ok::ui::Image(slot_widget.ptr(), &Zoner::ISpaceScreenGUI::o().GetIconCache(_icon_cache_slots_middle_id, 1, 0), 234.f + 2.f, 194.f + 2.f);

				if (Zoner::ISpaceScreenGUI::o().GetDragAndDropItem() == equipment_slots_middle[1])
				{
					ok::ui::PopEffect_Fade();
				}
			}

			if (equipment_slots_middle[2] == nullptr)
			{
				ok::ui::Image(slot_widget.ptr(), &spr_empty_slot_32, 60.f, 248.f);
			}
			else
			{
				ok::ui::Image(slot_widget.ptr(), &spr_full_slot_32, 60.f, 248.f);

				if (Zoner::ISpaceScreenGUI::o().GetDragAndDropItem() == equipment_slots_middle[2])
				{
					ok::ui::PushEffect_Fade(0.5f);
				}

				ok::ui::Image(slot_widget.ptr(), &Zoner::ISpaceScreenGUI::o().GetIconCache(_icon_cache_slots_middle_id, 2, 0), 60.f + 2.f, 248.f + 2.f);

				if (Zoner::ISpaceScreenGUI::o().GetDragAndDropItem() == equipment_slots_middle[2])
				{
					ok::ui::PopEffect_Fade();
				}
			}

			if (equipment_slots_middle[3] == nullptr)
			{
				ok::ui::Image(slot_widget.ptr(), &spr_empty_slot_32, 234.f, 248.f);
			}
			else
			{
				ok::ui::Image(slot_widget.ptr(), &spr_full_slot_32, 234.f, 248.f);

				if (Zoner::ISpaceScreenGUI::o().GetDragAndDropItem() == equipment_slots_middle[3])
				{
					ok::ui::PushEffect_Fade(0.5f);
				}

				ok::ui::Image(slot_widget.ptr(), &Zoner::ISpaceScreenGUI::o().GetIconCache(_icon_cache_slots_middle_id, 3, 0), 234.f + 2.f, 248.f + 2.f);

				if (Zoner::ISpaceScreenGUI::o().GetDragAndDropItem() == equipment_slots_middle[3])
				{
					ok::ui::PopEffect_Fade();
				}
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

						if (Zoner::ISpaceScreenGUI::o().GetDragAndDropItem() == equipment_slots_down[i + _equipment_slots_down_offset])
						{
							ok::ui::PushEffect_Fade(0.5f);
						}

						ok::ui::Image(slot_widget.ptr(), &Zoner::ISpaceScreenGUI::o().GetIconCache(_icon_cache_slots_down_id, i, 0), i * 43.f + 2.f, 2.f);

						if (Zoner::ISpaceScreenGUI::o().GetDragAndDropItem() == equipment_slots_down[i + _equipment_slots_down_offset])
						{
							ok::ui::PopEffect_Fade();
						}
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
					if (ok::ui::ws().on_activate)
					{
						Zoner::IItem* _drag_and_drop_item = Zoner::ISpaceScreenGUI::o().GetDragAndDropItem();
						Zoner::IItem* _highlighted_slot_item = equipment_slots_down[highlighted_slot_index + _equipment_slots_down_offset];

						if (_drag_and_drop_item != nullptr)
						{
							//placement logic
							if (_highlighted_slot_item == nullptr)
							{
								//place in empty slot
								//if both sides allowed this operation
								if (ItemInAllowed(_drag_and_drop_item) &&
									(_drag_and_drop_item->this_item_owner == nullptr || _drag_and_drop_item->this_item_owner->ItemOutAllowed(_drag_and_drop_item)))
								{
									if (_drag_and_drop_item->this_item_owner != nullptr)
									{
										_drag_and_drop_item->this_item_owner->ItemOut(_drag_and_drop_item);
									}

									ItemIn(_drag_and_drop_item);

									equipment_slots_down[highlighted_slot_index + _equipment_slots_down_offset] = _drag_and_drop_item;

									//cache icon
									if (_drag_and_drop_item->_blueprint_item != nullptr)
										Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_slots_down_id, _drag_and_drop_item->_blueprint_item, highlighted_slot_index, 0);
									else if (_drag_and_drop_item->_blueprint_spacecraft != nullptr)
										Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_slots_down_id, _drag_and_drop_item->_blueprint_spacecraft, highlighted_slot_index, 0);

									if (Zoner::ISpaceScreenGUI::o().IsDragAndDropItemFromInspector())
									{
										Zoner::ISpaceScreenGUI::o().RemoveDragAndDropItemFromInspector();
									}

									Zoner::ISpaceScreenGUI::o().SetDragAndDropItem(nullptr);
								}
								else
								{
									//cant move item, show effect
								}
							}
							else
							{
								//place item in full slot
								if (_highlighted_slot_item == _drag_and_drop_item)
								{
									//Place container back to its slot
									//if item from inspector
									if (Zoner::ISpaceScreenGUI::o().IsDragAndDropItemFromInspector())
									{
										Zoner::ISpaceScreenGUI::o().RemoveDragAndDropItemFromInspector();
									}

									Zoner::ISpaceScreenGUI::o().SetDragAndDropItem(nullptr);
								}
								else
								{
									//if both sides allowed this operation (exchange requires both sides to exist!)
									if (ItemInAllowed(_drag_and_drop_item, true, _highlighted_slot_item) &&
										(_drag_and_drop_item->this_item_owner != nullptr &&
											_drag_and_drop_item->this_item_owner->ItemOutAllowed(_drag_and_drop_item, true, _highlighted_slot_item)))
									{
										_drag_and_drop_item->this_item_owner->ItemOut(_drag_and_drop_item, true, _highlighted_slot_item);
										ItemIn(_drag_and_drop_item, true, _highlighted_slot_item);

										Zoner::IItem* exchanged_item = _highlighted_slot_item;
										equipment_slots_down[highlighted_slot_index + _equipment_slots_down_offset] = _drag_and_drop_item;

										//cache icon
										if (_drag_and_drop_item->_blueprint_item != nullptr)
											Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_slots_down_id, _drag_and_drop_item->_blueprint_item, highlighted_slot_index, 0);
										else if (_drag_and_drop_item->_blueprint_spacecraft != nullptr)
											Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_slots_down_id, _drag_and_drop_item->_blueprint_spacecraft, highlighted_slot_index, 0);


										if (Zoner::ISpaceScreenGUI::o().IsDragAndDropItemFromInspector())
										{
											Zoner::ISpaceScreenGUI::o().RemoveDragAndDropItemFromInspector();
										}

										//pickup exchanged item for drag and drop
										Zoner::ISpaceScreenGUI::o().SetDragAndDropItem(exchanged_item);
									}
									else
									{
										//cant move item, show effect
									}
								}
							}
						}
						else
						{
							//pickup logic
							if (_highlighted_slot_item == nullptr)
							{
								//this slot is empty, do nothing
							}
							else
							{
								//slot is full, pickup item
								Zoner::ISpaceScreenGUI::o().SetDragAndDropItem(_highlighted_slot_item);
							}
						}
					}
				}
			}
			ok::ui::PopTranslate();


			//down slots left right offset buttons
			//left
			if (_equipment_slots_down_offset > 0)
			{
				ok::ui::PushTranslate(41.f, 307.f);
				{
					ok::ui::PushNonActivable(false);
					ok::ui::Dummy(offset_button_widget_left_down.ptr(), 0, 0, 20, 35);
					ok::ui::PopNonActivable();

					if (ok::ui::ws().mouse_down)
					{
						if (ok::ui::ws().on_activate)
						{
							//act
							_equipment_slots_down_offset--;
							if (_equipment_slots_down_offset < 0) _equipment_slots_down_offset = 0;
							else
							{
								Zoner::ISpaceScreenGUI::o().MoveIconsInsideCache(_icon_cache_slots_down_id, 1, 0);

								if (equipment_slots_down[_equipment_slots_down_offset] != nullptr)
								{
									if (equipment_slots_down[_equipment_slots_down_offset]->_blueprint_item)
									{
										Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_slots_down_id, equipment_slots_down[_equipment_slots_down_offset]->_blueprint_item, 0, 0);
									}
									else if (equipment_slots_down[_equipment_slots_down_offset]->_blueprint_spacecraft)
									{
										Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_slots_down_id, equipment_slots_down[_equipment_slots_down_offset]->_blueprint_spacecraft, 0, 0);
									}
								}

							}
						}
						ok::ui::Image(offset_button_widget_left_down.ptr(), &spr_offset_btn_left_down);
					}
					else
					{
						ok::ui::Image(offset_button_widget_left_down.ptr(), &spr_offset_btn_left);
					}
				}
				ok::ui::PopTranslate();
			}
			else
			{
				ok::ui::PushTranslate(41.f, 307.f);
				{
					ok::ui::PushEffect_Fade(0.75f);
					ok::ui::Image(offset_button_widget_left_down.ptr(), &spr_offset_btn_left);
					ok::ui::PopEffect_Fade();
				}
				ok::ui::PopTranslate();
			}

			if (_equipment_slots_down_offset + 4 < static_cast<int>(equipment_slots_down.size()) - 1)
			{
				//right
				ok::ui::PushTranslate(268.f, 307.f);
				{
					ok::ui::PushNonActivable(false);
					ok::ui::Dummy(offset_button_widget_right_down.ptr(), 0, 0, 20, 35);
					ok::ui::PopNonActivable();

					if (ok::ui::ws().mouse_down)
					{
						if (ok::ui::ws().on_activate)
						{
							//act
							_equipment_slots_down_offset++;
							if (_equipment_slots_down_offset + 4 >= static_cast<int>(equipment_slots_down.size())) _equipment_slots_down_offset--;
							else
							{
								Zoner::ISpaceScreenGUI::o().MoveIconsInsideCache(_icon_cache_slots_down_id, -1, 0);

								if (equipment_slots_down[_equipment_slots_down_offset + 4] != nullptr)
								{
									if (equipment_slots_down[_equipment_slots_down_offset + 4]->_blueprint_item)
									{
										Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_slots_down_id, equipment_slots_down[_equipment_slots_down_offset + 4]->_blueprint_item, 4, 0);
									}
									else if (equipment_slots_down[_equipment_slots_down_offset + 4]->_blueprint_spacecraft)
									{
										Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_slots_down_id, equipment_slots_down[_equipment_slots_down_offset + 4]->_blueprint_spacecraft, 4, 0);
									}
								}
							}
						}
						ok::ui::Image(offset_button_widget_right_down.ptr(), &spr_offset_btn_right_down);
					}
					else
					{
						ok::ui::Image(offset_button_widget_right_down.ptr(), &spr_offset_btn_right);
					}
				}
				ok::ui::PopTranslate();
			}
			else
			{
				ok::ui::PushTranslate(268.f, 307.f);
				{
					ok::ui::PushEffect_Fade(0.75f);
					ok::ui::Image(offset_button_widget_right_down.ptr(), &spr_offset_btn_right);
					ok::ui::PopEffect_Fade();
				}
				ok::ui::PopTranslate();
			}


			//container slot
			{
				ok::ui::PushNonActivable(false);
				ok::ui::Dummy(slot_widget.ptr(), 298.f, 223.f, 36.f, 36.f);
				if (ok::ui::ws().mouse_inside)
				{
					if (ok::ui::ws().on_activate)
					{
						if (Zoner::ISpaceScreenGUI::o().GetDragAndDropItem() == container_slot)
						{
							//Place container back to its slot
							//if item from inspector
							if (Zoner::ISpaceScreenGUI::o().IsDragAndDropItemFromInspector())
							{
								Zoner::ISpaceScreenGUI::o().RemoveDragAndDropItemFromInspector();
							}

							Zoner::ISpaceScreenGUI::o().SetDragAndDropItem(nullptr);
						}
						else
						{
							if (Zoner::ISpaceScreenGUI::o().GetDragAndDropItem() == nullptr)
							{
								//pick up container from slot
								Zoner::ISpaceScreenGUI::o().SetDragAndDropItem(container_slot);
							}
							else
							{
								//place some item in container
							}
						}
					}
				}
				ok::ui::PopNonActivable();

				if (container_slot == nullptr)
				{
					ok::ui::Image(slot_widget.ptr(), &spr_empty_slot_32, 298.f, 223.f);
				}
				else
				{
					ok::ui::Image(slot_widget.ptr(), &spr_full_slot_32, 298.f, 223.f);

					if (Zoner::ISpaceScreenGUI::o().GetDragAndDropItem() == container_slot)
					{
						ok::ui::PushEffect_Fade(0.5f);
					}

						ok::ui::Image(slot_widget.ptr(), &Zoner::ISpaceScreenGUI::o().GetIconCache(_icon_cache_container_slot_id, 0, 0), 298.f + 2.f, 223.f + 2.f);
					
					if (Zoner::ISpaceScreenGUI::o().GetDragAndDropItem() == container_slot)
					{
						ok::ui::PopEffect_Fade();
					}
				}
			}
		}
		ok::ui::PopTranslate();

		
	}
	ok::ui::PopTranslate();
	ok::ui::PopNonActivable();


	//spacecraft ui changes here

	//extend equipment up slots
	if (equipment_slots_up[_equipment_slots_up_offset+4] != nullptr &&
		_equipment_slots_up_offset + 4 == equipment_slots_up.size() - 1)
	{
		equipment_slots_up.push_back(nullptr);
	}

	//shrink equipment up slots
	while (
		equipment_slots_up.size() > 5 &&
		equipment_slots_up[equipment_slots_up.size() - 1] == nullptr &&
		equipment_slots_up[equipment_slots_up.size() - 2] == nullptr)
	{
		equipment_slots_up.pop_back();

		if (_equipment_slots_up_offset + 4 >= static_cast<int>(equipment_slots_up.size()))
		{
			_equipment_slots_up_offset--;
			if (_equipment_slots_up_offset < 0) _equipment_slots_up_offset = 0;
			else
			{
				Zoner::ISpaceScreenGUI::o().MoveIconsInsideCache(_icon_cache_slots_up_id, 1, 0);

				if (equipment_slots_up[_equipment_slots_up_offset] != nullptr)
				{
					if (equipment_slots_up[_equipment_slots_up_offset]->_blueprint_item)
					{
						Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_slots_up_id, equipment_slots_up[_equipment_slots_up_offset]->_blueprint_item, 0, 0);
					}
					else if (equipment_slots_up[_equipment_slots_up_offset]->_blueprint_spacecraft)
					{
						Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_slots_up_id, equipment_slots_up[_equipment_slots_up_offset]->_blueprint_spacecraft, 0, 0);
					}
				}

			}
		}
	}

	//extend equipment down slots
	if (equipment_slots_down[_equipment_slots_down_offset + 4] != nullptr &&
		_equipment_slots_down_offset + 4 == equipment_slots_down.size() - 1)
	{
		equipment_slots_down.push_back(nullptr);
	}

	//shrink equipment down slots
	while (
		equipment_slots_down.size() > 5 &&
		equipment_slots_down[equipment_slots_down.size() - 1] == nullptr &&
		equipment_slots_down[equipment_slots_down.size() - 2] == nullptr)
	{
		equipment_slots_down.pop_back();

		if (_equipment_slots_down_offset + 4 >= static_cast<int>(equipment_slots_down.size()))
		{
			_equipment_slots_down_offset--;
			if (_equipment_slots_down_offset < 0) _equipment_slots_down_offset = 0;
			else
			{
				Zoner::ISpaceScreenGUI::o().MoveIconsInsideCache(_icon_cache_slots_down_id, 1, 0);

				if (equipment_slots_down[_equipment_slots_down_offset] != nullptr)
				{
					if (equipment_slots_down[_equipment_slots_down_offset]->_blueprint_item)
					{
						Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_slots_down_id, equipment_slots_down[_equipment_slots_down_offset]->_blueprint_item, 0, 0);
					}
					else if (equipment_slots_down[_equipment_slots_down_offset]->_blueprint_spacecraft)
					{
						Zoner::ISpaceScreenGUI::o().CacheIcon(_icon_cache_slots_down_id, equipment_slots_down[_equipment_slots_down_offset]->_blueprint_spacecraft, 0, 0);
					}
				}

			}
		}
	}
	//ok::ui::EndUI();
}

void Zoner::ItemSpacecraft::DestroyUI()
{
	//order is important here for performance reason
	Zoner::ISpaceScreenGUI::o().ReleaseIconsCache(_icon_cache_container_slot_id);
	Zoner::ISpaceScreenGUI::o().ReleaseIconsCache(_icon_cache_slots_middle_id);
	Zoner::ISpaceScreenGUI::o().ReleaseIconsCache(_icon_cache_slots_down_id);
	Zoner::ISpaceScreenGUI::o().ReleaseIconsCache(_icon_cache_slots_up_id);
}

void Zoner::ItemSpacecraft::ItemIn(Zoner::IItem * item, bool exchange, const Zoner::IItem * exchange_item)
{
	item->this_item_owner = this;

	if (exchange)
	{
		//on exchange just rewrite existing element in equipment_items
		auto it = std::find(equipment_items.begin(), equipment_items.end(), exchange_item);
		*it = item;
	}
	else
	{
		//items erased from equipment_items on item out, so no need to search them in equipment_items, its guaranteed that this items will not be here
		equipment_items.push_back(item);

		//if this item already was here dont push it again, just left it where it was in equipment_items
		/*auto it = std::find(equipment_items.begin(), equipment_items.end(), item);

		if (it == equipment_items.end())
		{
			equipment_items.push_back(item);
		}*/
	}
	
}

void Zoner::ItemSpacecraft::ItemOut(const Zoner::IItem * item, bool exchange, Zoner::IItem * exchange_item)
{
	auto it = std::find(equipment_slots_up.begin(), equipment_slots_up.end(), item);
	std::vector<Zoner::IItem*>* slots_ptr = nullptr;
	int _slots_icon_cache_id = -1;
	int _slots_icon_cache_offset = 0;

	if (it != equipment_slots_up.end())
	{
		slots_ptr = &equipment_slots_up;
		_slots_icon_cache_id = _icon_cache_slots_up_id;
		_slots_icon_cache_offset = _equipment_slots_up_offset;
	}
	else
	{
		it = std::find(equipment_slots_middle.begin(), equipment_slots_middle.end(), item);

		if (it != equipment_slots_middle.end())
		{
			slots_ptr = &equipment_slots_middle;
			_slots_icon_cache_id = _icon_cache_slots_middle_id;
			_slots_icon_cache_offset = 0;
		}
		else
		{
			it = std::find(equipment_slots_down.begin(), equipment_slots_down.end(), item);

			if (it != equipment_slots_down.end())
			{
				slots_ptr = &equipment_slots_down;
				_slots_icon_cache_id = _icon_cache_slots_down_id;
				_slots_icon_cache_offset = _equipment_slots_down_offset;
			}
		}
	}

	if (slots_ptr != nullptr)
	{
		int position = it - slots_ptr->begin();
		if (exchange)
		{
			(*slots_ptr)[position] = exchange_item;
			exchange_item->this_item_owner = this;

			if (exchange_item->_blueprint_item != nullptr)
				Zoner::ISpaceScreenGUI::o().CacheIcon(_slots_icon_cache_id, exchange_item->_blueprint_item, position - _slots_icon_cache_offset, 0);
			else if (exchange_item->_blueprint_spacecraft != nullptr)
				Zoner::ISpaceScreenGUI::o().CacheIcon(_slots_icon_cache_id, exchange_item->_blueprint_spacecraft, position - _slots_icon_cache_offset, 0);

			//reset item in equipment_items to exchange_item
			it = std::find(equipment_items.begin(), equipment_items.end(), item);
			*it = exchange_item;
		}
		else
		{
			(*slots_ptr)[position] = nullptr;
			equipment_items.erase(std::find(equipment_items.begin(), equipment_items.end(), item));
		}
	}
}

bool Zoner::ItemSpacecraft::ItemInAllowed(Zoner::IItem * item, bool exchange, Zoner::IItem * exchange_item)
{
	auto _item = static_cast<Zoner::IItem*>(&gameObject());
	Zoner::UID& _item_uid = *_item;

	if (_item_uid == *item || item->FindItem(*_item) != nullptr)
	{
		//self nesting cycle detected, cant allow this
		return false;
	}

	return true;
}

bool Zoner::ItemSpacecraft::ItemOutAllowed(Zoner::IItem * item, bool exchange, Zoner::IItem * exchange_item)
{
	if (item == container_slot) return false;

	return true;
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
