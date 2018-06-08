#pragma once

#include "IItem.h"
#include "..\..\Assets.h"
#include "ISpaceScreenGUI.h"
#include "..\..\UI.h"
#include "..\..\GameObject.h"
#include "IShip.h"

namespace Zoner
{
	class ItemCargoHold : public Zoner::IItemBehaviour
	{
	public:
		void InitBlocks(int blocks_count); //min 1 max 4, each block is 5x2 items

		void PackItems();
		void SwapItems(Zoner::IItem*& item_to_swap, int x, int y);
		Zoner::IItem* PlaceItem(Zoner::IItem* item_to_place, int x, int y);
		Zoner::IItem* FindItem(int x, int y);
		Zoner::IItem* FindItem(int index);
		Zoner::IItem* FindItem(ok::String& item_name);

		std::vector<Zoner::IItem*>& Items();

		void SaveTo(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement& element) {};
		void LoadFrom(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement& element);

		Zoner::IItem* FindItem(Zoner::UID& item_uid);
		void CollectItems(std::map<Zoner::UID, Zoner::IItem*>& _collection);

		void CreateUI();
		void UpdateUI(float dt);
		void DestroyUI();
	private:
		std::vector<Zoner::IItem*> _items; //5x2 min, 4x5x2 max
	protected:
	};

	class ItemSpacecraft : public Zoner::IItemBehaviour
	{
	public:
		void SaveTo(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement& element) {};
		void LoadFrom(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement& element);

		Zoner::IItem* FindItem(Zoner::UID& item_uid);
		void CollectItems(std::map<Zoner::UID, Zoner::IItem*>& _collection);

		int capacity;
		int capacity_left;
		Zoner::IItem* container_slot = nullptr;
		std::vector<Zoner::IItem*> equipment_slots;

		void CreateUI();
		void UpdateUI(float dt);
		void DestroyUI();

		//void SetLayout(int layout_index);
	private:
		int _icon_cache_size = 1;
		int _icon_cache_id;
	protected:
	};

	class Item : public Zoner::IItem
	{
	public:
		void SaveTo(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement& element) {};
		void LoadFrom(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement& element) {};

		Zoner::IItem* FindItem(Zoner::UID& item_uid);
		void CollectItems(std::map<Zoner::UID, Zoner::IItem*>& _collection);

		void CreateUI();
		void UpdateUI(float dt);
		void DestroyUI();
	protected:
	private:
	};

	class ItemBuilder : public Zoner::IItemBuilder
	{
	public:
		ItemBuilder();

		Zoner::IItem* BuildFromRecipe(ok::String& recipe);
	private:
		std::unordered_map<std::string, std::function<Zoner::IItemBehaviour*()>> _behaviours_allocator;
	};
}