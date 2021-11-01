#pragma once

#include "ITDSGame.h"

namespace NSTDS
{
	class TDSGame : public NSTDS::ITDSGame
	{
	public:
		TDSGame();
		void Init();
		void Update(float dt);
	protected:
	private:
	};
}