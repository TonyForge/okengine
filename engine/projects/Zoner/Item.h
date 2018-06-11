#pragma once

#include "..\..\Assets.h"
#include "..\..\UI.h"
#include "..\..\GameObject.h"
#include "..\..\Transform.h"

#include "IItem.h"
#include "ISpaceScreenGUI.h"
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

		void ItemIn(Zoner::IItem* item, bool exchange = false, const Zoner::IItem* exchange_item = nullptr);
		void ItemOut(const Zoner::IItem* item, bool exchange = false, Zoner::IItem* exchange_item = nullptr);
		bool ItemInAllowed(Zoner::IItem* item, bool exchange = false, Zoner::IItem* exchange_item = nullptr);
		bool ItemOutAllowed(Zoner::IItem* item, bool exchange = false, Zoner::IItem* exchange_item = nullptr);
	private:
		std::vector<Zoner::IItem*> _items; //5x2 min, 4x5x2 max
		int _icon_cache_size;
		int _icon_cache_id;
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
		std::vector<Zoner::IItem*> equipment_items; //dense list of items

		//sparse lists for ui only
		std::vector<Zoner::IItem*> equipment_slots_up;
		std::vector<Zoner::IItem*> equipment_slots_middle;
		std::vector<Zoner::IItem*> equipment_slots_down;

		void CreateUI();
		void UpdateUI(float dt);
		void DestroyUI();

		void ItemIn(Zoner::IItem* item, bool exchange = false, const Zoner::IItem* exchange_item = nullptr);
		void ItemOut(const Zoner::IItem* item, bool exchange = false, Zoner::IItem* exchange_item = nullptr);
		bool ItemInAllowed(Zoner::IItem* item, bool exchange = false, Zoner::IItem* exchange_item = nullptr);
		bool ItemOutAllowed(Zoner::IItem* item, bool exchange = false, Zoner::IItem* exchange_item = nullptr);
		//void SetLayout(int layout_index);
	private:
		int _icon_cache_slots_up_id;
		int _icon_cache_slots_down_id;
		int _icon_cache_slots_middle_id;
		int _icon_cache_container_slot_id;
		//int _icon_cache_size = 15;
		//int _icon_cache_id;
		int _equipment_slots_up_offset = 0;
		int _equipment_slots_down_offset = 0;
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