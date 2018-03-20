#pragma once

#include "..\..\GameObject.h"
#include "..\..\Rect2D.h"
#include "IGame.h"
#include "IShip.h"
#include "ISpace.h"

namespace Zoner
{
	class Space : public Zoner::ISpace
	{
	public:
		std::vector<Zoner::IShip*> visitors;

		//std::vector<Zoner::IShip*> planets;
		//std::vector<Zoner::IShip*> stars;

		std::vector<Zoner::SpaceConnection*> connections;

		ok::Rect2Df borders;

		void PassTime(float hours_passed);
		void ApplyPassedTime();
	protected:
	private:
	};
}