#pragma once

#include "Ship.h"
#include "ISpace.h"
#include "..\..\LineBatch.h"

namespace Zoner
{
	class JumpHole : public Zoner::Ship
	{
	public:
		JumpHole();

		glm::vec2 radius;
		Zoner::ISpace* destination;
		glm::vec2 destination_position;

		Zoner::Collision::Point Pick(glm::vec3 world_position);

		ok::graphics::LineBatch debug_draw;

		void Update(float dt);

		void* GetPtr(int id);
	private:
	protected:
	};
}