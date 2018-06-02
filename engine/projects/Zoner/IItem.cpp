#include "IItem.h"

Zoner::IItemBuilder* Zoner::IItemBuilder::_instance = nullptr;

Zoner::IItemBuilder & Zoner::IItemBuilder::o()
{
	return *_instance;
}

ok::GameObject * Zoner::ItemBlueprint::Duplicate(ok::GameObject * _clone)
{
	Zoner::ItemBlueprint* __clone;

	if (_clone == nullptr)
	{
		__clone = new Zoner::ItemBlueprint();
	}
	else
	{
		__clone = static_cast<Zoner::ItemBlueprint*>(_clone);
	}

	ok::GameObject::Duplicate(__clone);

	//copy this to __clone

	return __clone;
}
