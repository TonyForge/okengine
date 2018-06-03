#include "Item.h"

void Zoner::ItemCargoHold::InitBlocks(int blocks_count)
{
	_items.resize(10 * blocks_count);
	std::fill(_items.begin(), _items.end(), nullptr);
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

	auto elem = element.FirstChildElement("equipment_slots");
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
