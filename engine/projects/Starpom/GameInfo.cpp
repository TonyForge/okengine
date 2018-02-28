#include "GameInfo.h"

Starpom::GameInfo & Starpom::GameInfo::instance()
{
	static Starpom::GameInfo s;
	return s;
}

Starpom::GameInfo & Starpom::GameInfo::o()
{
	return instance();
}

Starpom::GameMode Starpom::GameInfo::GetMode()
{
	return _game_mode;
}
