#pragma once

#include "IGame.h"

namespace Kripta
{
	class Game : public Kripta::IGame
	{
	public:
		Game();
		void Init();
		void Update(float dt);
	};
}