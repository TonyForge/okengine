#pragma once

#include "..\..\UI.h"
#include "..\..\SpriteBatch.h"
#include "..\..\RenderTarget.h"
#include "..\..\Assets.h"
#include "..\..\String.h"
#include "..\..\LayeredRenderer.h"
#include "..\..\Camera.h"
#include "..\..\Transform.h"
#include "..\..\Texture.h"
#include "IGame.h"
#include "IShip.h"

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

		void _CacheIcon(ok::GameObject* blueprint, float scale, int slot_x, int slot_y);
		ok::graphics::SpriteInfo _GetIconCache(int slot_x, int slot_y);
		
		ok::graphics::RenderTarget* _icons_cache_64px = nullptr; //512x512, 32x32 in double resolution for supersampling
		ok::graphics::Texture* _icons_cache_64px_tex = nullptr;
	};
}