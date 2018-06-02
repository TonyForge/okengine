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
	for (auto item : _items)
	{
		if (item != nullptr && item->gameObject().name == item_name)
		{
			return item;
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
