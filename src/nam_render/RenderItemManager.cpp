#include "pch.h"
#include "RenderItemManager.h"
#include "Mesh.h"
#include "Sprite.h"
#include "Text.h"

namespace nam
{
	Queue<u32> RenderItemManager::m_freeIDs = {};
	
	RenderItemManager::RenderItemManager()
	{
		for (size_t i = 0; i < MAX_RENDER_ITEMS; i++)
		{
			m_freeIDs.push(static_cast<u32>(i));
		}
	}

	RenderItemManager::~RenderItemManager()
	{
		for (auto& [id, p_item] : m_renderItems)
		{
			if(p_item)
				delete p_item;
		}

		m_renderItems.clear();
	}

	bool RenderItemManager::RemoveRenderItem(u32 itemID)
	{
		auto it = m_renderItems.find(itemID);

		if (it == m_renderItems.end())
		{
			_ASSERT(false && "RenderItem not found ! Can't Erase !");
			return false;
		}

		ARenderItem* p_item = it->second;

		if (p_item == nullptr)
			return false;

		m_freeIDs.push(p_item->m_id);
		delete p_item;
		m_renderItems.erase(it);

		return true;
	}

	void RenderItemManager::SetRenderItemTag(ARenderItem* renderItem, size tag)
	{
		m_taggedRenderItems[tag] = renderItem;
	}

	ARenderItem* RenderItemManager::GetRenderItem(u32 itemID)
	{
		auto it = m_renderItems.find(itemID);

		if (it == m_renderItems.end())
		{
			_ASSERT(false && "RenderItem not found ! Can't Get !");
			return nullptr;
		}

		return it->second;
	}
	ARenderItem* RenderItemManager::GetRenderItemOfTag(size tag)
	{
		auto it = m_taggedRenderItems.find(tag);

		if (it == m_taggedRenderItems.end())
		{
			_ASSERT(false && "RenderItem not found ! Can't Get !");
			return nullptr;
		}

		return it->second;
	}
}

