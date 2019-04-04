#pragma once

#include "Interfaces.h"
#include "StarSystem.h"
#include "Planet.h"

namespace ZonerX
{
	class Game : public ZonerX::IGame
	{
	public:
		Game();
		void Init();
		void Update(float dt);
	protected:
	private:
	};
}