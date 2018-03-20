#pragma once

#include "IGame.h"
#include "IShip.h"
#include "ISpace.h"

namespace Zoner
{


	class Ship : public Zoner::IShip
	{
	public:
		void PassTime(float hours_passed);
		void ApplyPassedTime();
		Zoner::ISpace* location;
	protected:
	private:
	};
}