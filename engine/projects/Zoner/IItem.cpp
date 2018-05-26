#include "IItem.h"


Zoner::IItemBuilder & Zoner::IItemBuilder::o()
{
	return *_instance;
}
