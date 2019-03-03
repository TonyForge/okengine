#pragma once

#include "IGame.h"

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