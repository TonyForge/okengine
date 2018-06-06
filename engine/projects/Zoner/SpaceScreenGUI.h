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
#include "..\..\MeshRenderer.h"
#include "IGame.h"
#include "IShip.h"
#include "IItem.h"

namespace Zoner
{
	class SpaceScreenGUI
	{
	public:

		static void Update(float dt);
		static void Update_Inspector(float dt);
		static void Update_Item_Spacecraft(float dt);
		static void Update_Item_Container(float dt);

		static void Create_Inspector();
		static void Destroy_Inspector();

		static Zoner::SpaceScreenGUI& instance();
		static Zoner::SpaceScreenGUI& o();
	protected:
	private:
		SpaceScreenGUI() {}
		~SpaceScreenGUI() {}
		Zoner::SpaceScreenGUI(Zoner::SpaceScreenGUI const&) {}
		Zoner::SpaceScreenGUI& operator= (Zoner::SpaceScreenGUI const&) {}

		void _CacheIcon(ok::GameObject* blueprint, int slot_x, int slot_y);
		void _CalculateBlueprintBounds(ok::Transform* part, glm::vec4 & bounds);
		ok::graphics::SpriteInfo _GetIconCache(int slot_x, int slot_y);

		//512x512, 32x32 in double resolution for supersampling, 8x8 slots
		//0..4 - small slots of inspector
		//5 - drag n drop icon
		//6 .. 63 - other icons (41 for container, 15 for ship etc)
		ok::graphics::RenderTarget* _icons_cache_64px = nullptr; 
		ok::graphics::Texture* _icons_cache_64px_tex = nullptr;

		bool _initialized = false;

		std::vector<Zoner::UID> _inspector_items;
		std::vector<Zoner::IItem*> _inspector_items_in_slots;
		Zoner::IItem* _inspector_big_slot_item = nullptr;
		bool _inspector_recache_icons = false;

		Zoner::IItem* _drag_and_drop_item = nullptr;
		Zoner::UID* _drag_and_drop_item_shortcut = nullptr; //example of shortcuts is inspector small slot (keeps shortcut, not item itself)
		
		float rot1 = 0.f;
		//void* _drag_and_drop_item_location = nullptr;
		//bool _drag_and_drop_item_live_in_inspector = false;
	};
}