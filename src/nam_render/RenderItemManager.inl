#include "RenderItemManager.h"
#pragma once

namespace nam
{
	template<typename T>
	inline T* RenderItemManager::CreateRenderItem()
	{
		static_assert(std::is_base_of_v<ARenderItem, T>, "CreateRenderItem<T> must derive from ARenderItem");

		if (m_freeIDs.empty())
		{
			_ASSERT(false && "No freeIds to create a new RenderItem");
			return nullptr;
		}

		u32 newID = m_freeIDs.front();
		m_freeIDs.pop();

		T* t = new T();
		t->m_id = newID;

		m_renderItems[newID] = t;

		return t;
	}

	template<typename T>
	inline bool RenderItemManager::RemoveRenderItem(T*& p_item)
	{
		if (p_item == nullptr)
			return false;

		if (RemoveRenderItem(p_item->m_id) == false)
			return false;

		p_item = nullptr;

		return true;
	}
}