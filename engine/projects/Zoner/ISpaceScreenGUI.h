#pragma once

#include "IItem.h"
#include "..\..\SpriteBatch.h"
#include "..\..\GameObject.h"

namespace Zoner
{
	class ISpaceScreenGUI
	{
	public:
		static Zoner::ISpaceScreenGUI& o();

		virtual int ReserveIconsCache(int size_x, int size_y) = 0;
		virtual void CacheIcon(int cache_id, ok::GameObject* blueprint, int slot_x, int slot_y) = 0;
		virtual ok::graphics::SpriteInfo GetIconCache(int cache_id, int slot_x, int slot_y) = 0;
		virtual void ReleaseIconsCache(int cache_id) = 0;
		virtual void MoveIconsInsideCache(int cache_id, int shift_x, int shift_y) = 0;

		virtual void SetDragAndDropItem(Zoner::IItem* item) = 0;
		virtual Zoner::IItem* GetDragAndDropItem() = 0;
		virtual bool IsDragAndDropItemFromInspector() = 0;
		virtual void RemoveDragAndDropItemFromInspector() = 0;
	private:
	protected:
		static Zoner::ISpaceScreenGUI* _instance;
	};
}