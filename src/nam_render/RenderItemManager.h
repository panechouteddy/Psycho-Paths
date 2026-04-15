#pragma once
#include <ContainerSequential.h>
#include "ARenderItem.h"
#include "Constants.h"

namespace nam
{
	struct ARenderItem;
	struct Mesh;
	struct Sprite;
	struct Text;

	class RenderItemManager
	{
		static Queue<u32> m_freeIDs;
		UnMap<u32, ARenderItem*> m_renderItems = {};
		UnMap<size, ARenderItem*> m_taggedRenderItems = {};

	public:
		RenderItemManager();
		~RenderItemManager();

		template<typename T>
		T* CreateRenderItem();

		bool RemoveRenderItem(u32 itemID);

		//Set the pointer to nullptr if succes
		template<typename T>
		bool RemoveRenderItem(T*& p_item);

		void SetRenderItemTag(ARenderItem* renderItem, size tag);

		ARenderItem* GetRenderItem(u32 itemID);
		ARenderItem* GetRenderItemOfTag(size tag);
	};
}

#include "RenderItemManager.inl"