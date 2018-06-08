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
#include "ISpaceScreenGUI.h"

namespace Zoner
{
	class SpaceScreenGUI : public Zoner::ISpaceScreenGUI
	{
	public:

		static void Update(float dt);
		static void Update_Inspector(float dt);

		static void Create_Inspector();
		static void Destroy_Inspector();

		static Zoner::SpaceScreenGUI& instance();
		static Zoner::SpaceScreenGUI& o();

		int ReserveIconsCache(int size_x, int size_y);
		void ReleaseIconsCache(int cache_id);
		void CacheIcon(int cache_id, ok::GameObject* blueprint, int slot_x, int slot_y);
		ok::graphics::SpriteInfo GetIconCache(int cache_id, int slot_x, int slot_y);
	protected:
	private:
		SpaceScreenGUI() { _instance = this; }
		~SpaceScreenGUI() {}
		Zoner::SpaceScreenGUI(Zoner::SpaceScreenGUI const&) { _instance = this; }
		Zoner::SpaceScreenGUI& operator= (Zoner::SpaceScreenGUI const&) {}

		void _CacheIcon(ok::GameObject* blueprint, int slot_x, int slot_y);
		void _CalculateBlueprintBounds(ok::Transform* part, glm::vec4 & bounds);
		ok::graphics::SpriteInfo _GetIconCache(int slot_x, int slot_y);
		void _MoveIconCache(int offset_from, int offset_to);

		bool _move_icon_cache_seq_enabled = false;
		void _MoveIconCacheSeqBegin();
		void _MoveIconCacheSeqEnd();

		//1024x1024, 32x32 in double resolution for supersampling, 16x16 slots
		//0..4 - small slots of inspector
		//5 - drag n drop icon
		//6 .. 255 - other icons (41 for container, 15 for ship etc)
		ok::graphics::RenderTarget* _icons_cache_64px = nullptr; 
		ok::graphics::Texture* _icons_cache_64px_tex = nullptr;

		const int _icons_cache_items_limit = 256;

		struct _IconsCacheReserve
		{
			int size_x, size_y, total_size;
			int offset;
			bool in_use = false;
		};

		std::vector<Zoner::SpaceScreenGUI::_IconsCacheReserve> _icons_cache_reserve_records;
		int _icons_cache_first_free_offset = 0;

		bool _initialized = false;

		std::vector<Zoner::UID> _inspector_items;
		std::vector<Zoner::IItem*> _inspector_items_in_slots;
		Zoner::IItem* _inspector_big_slot_item = nullptr;
		float _inspector_big_slot_item_rotation = 0.f;
		bool _inspector_recache_icons = false;
		int _inspector_icons_cache_id;


		Zoner::IItem* _drag_and_drop_item = nullptr;
		Zoner::UID* _drag_and_drop_item_shortcut = nullptr; //example of shortcuts is inspector small slot (keeps shortcut, not item itself)

		//void* _drag_and_drop_item_location = nullptr;
		//bool _drag_and_drop_item_live_in_inspector = false;
	};
}