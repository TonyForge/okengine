#pragma once

#include "..\..\GameObject.h"

namespace Zoner
{
	class ISpace : public ok::GameObject
	{
	public:
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