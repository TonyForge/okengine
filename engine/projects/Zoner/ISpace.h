#pragma once

#include "..\..\GameObject.h"
#include "..\..\Camera.h"
#include "IGame.h"

namespace Zoner
{
	class IShip;

	class ISpace : public ok::GameObject
	{
	public:
		ISpace();

		virtual void VisitorIn(Zoner::IShip* visitor) = 0;
		virtual void VisitorOut(Zoner::IShip* visitor) = 0;

		ok::graphics::Camera camera;

		virtual std::vector<Zoner::IShip*>& WhoIsThere(glm::vec2 space_xy) = 0;
		static std::vector<Zoner::IShip*> _who_is_there_container;
	protected:
	private:
	};

	class SpaceConnection
	{
	public:
		glm::vec3 position;
		glm::vec3 radius; //ellipsoids allowed
		Zoner::ISpace* destination;
	};
}