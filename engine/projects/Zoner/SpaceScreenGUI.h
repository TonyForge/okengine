#pragma once

#include "..\..\UI.h"
#include "..\..\SpriteBatch.h"
#include "..\..\Assets.h"
#include "..\..\String.h"
#include "IGame.h"

namespace Zoner
{
	class SpaceScreenGUI
	{
	public:

		static void Update(float dt);
		static void Update_Inventory(float dt);

		static Zoner::SpaceScreenGUI& instance();
		static Zoner::SpaceScreenGUI& o();
	protected:
	private:
		SpaceScreenGUI() {}
		~SpaceScreenGUI() {}
		Zoner::SpaceScreenGUI(Zoner::SpaceScreenGUI const&) {}
		Zoner::SpaceScreenGUI& operator= (Zoner::SpaceScreenGUI const&) {}
	};
}