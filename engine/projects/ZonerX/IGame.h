#pragma once

#include "..\..\Application.h"
#include "..\..\Assets.h"
#include "..\..\RenderTarget.h"

namespace ZonerX
{
	class Game;

	class IGame : public ok::Application
	{
	public:
		static ZonerX::IGame* instance;
	protected:
	private:
	};
}