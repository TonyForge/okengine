#include "IGame.h"

Zoner::IGame* Zoner::IGame::_instance;

Zoner::IGame & Zoner::IGame::o()
{
	return *_instance;
}
