#pragma once

#include "..\..\Application.h"
#include "..\..\Assets.h"
#include "..\..\RenderTarget.h"

namespace ZonerX
{
	class IActor;

	class IGame : public ok::Application
	{
	public:
		static ZonerX::IGame* instance;
		static ZonerX::IActor* current_star_system;
	protected:
	private:
	};
}