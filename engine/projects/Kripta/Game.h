#pragma once

#include "IGame.h"
#include "GameRoom.h"

namespace Kripta
{
	class Game : public Kripta::IGame
	{
	public:
		Game();
		void Init();
		void Update(float dt);
		ok::graphics::RenderTarget* GetScreenBuffer();
		virtual void LoadRoom(ok::String path);
		virtual std::pair<Kripta::IObject*, Kripta::IObject*> PickObjectFromGrid(int grid_x, int grid_y);

		Kripta::GameRoom room;
	};
}