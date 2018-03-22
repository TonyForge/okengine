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