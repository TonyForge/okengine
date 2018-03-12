#pragma once

#include "Ship.h"

namespace Starpom
{
	class Spaceship : public Starpom::Ship
	{
	public:
		Spaceship();

		void Update(float dt);
	};
}