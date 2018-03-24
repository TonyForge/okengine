#pragma once

#include "..\..\GameObject.h"
#include "..\..\LayeredRenderer.h"
#include "..\..\Rect2D.h"

#include "IGame.h"
#include "IShip.h"
#include "ISpace.h"
#include "Collision.h"

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
		void OnNewDay();

		void Update(float dt);

		void VisitorIn(Zoner::IShip* visitor);
		void VisitorOut(Zoner::IShip* visitor);

		std::vector<Zoner::IShip*>& WhoIsThere(glm::vec2 space_xy);
	protected:
	private:
	};
}