#pragma once

#include "..\..\String.h"
#include "..\..\GameObject.h"
#include "..\..\SpriteBatch.h"
#include "UID.h"

namespace Zoner
{
	class IItem : public ok::GameObject, public Zoner::UID
	{
	public:
		bool is_countable = false;
		int count = 1;

		//virtual void Save
	private:
	protected:
	};


	class ItemBlueprint : public ok::GameObject
	{
	public:
	private:
	protected:
	};

	class ItemBlueprintContainer : public ok::Behaviour
	{
	public:
	private:
		Zoner::ItemBlueprint* _blueprint;
	protected:
	};

	class ItemCargoHold : public ok::Behaviour
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
	private:
		std::vector<Zoner::IItem*> _items; //5x2 min, 4x5x2 max
	protected:
	};

	class ItemSpacecraft : public ok::Behaviour
	{
	public:
		int capacity;
		int capacity_left;
		Zoner::IItem* container_slot = nullptr;
		std::vector<Zoner::IItem*> equipment_slots;
	private:
	protected:
	};




}