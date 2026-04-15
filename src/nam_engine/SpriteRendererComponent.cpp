#include "pch.h"
#include "SpriteRendererComponent.h"
#include <RenderManager.h>

namespace nam
{
	Sprite* SpriteRendererComponent::CreateSpriteInstance()
	{
		mp_sprite = RENDER_MANAGER->GetRenderItemManager().CreateRenderItem<Sprite>();

		return mp_sprite;
	}

	void SpriteRendererComponent::SetSpriteInstance(Sprite* p_sprite)
	{
		if (p_sprite == nullptr)
		{
			_ASSERT(false && "You setted a nullptr Sprite");
			return;
		}

		if (p_sprite->GetID() == INVALID_ID)
		{
			_ASSERT(false && "YOU CAN'T CREATE RAW SPRITES INSTANCES, YOU NEED TO CREATE A SPRITE\nWITH THE RenderItemManager class : Use app->CreateEmptySprite()");
			return;
		}

		mp_sprite = p_sprite;
	}

	void SpriteRendererComponent::DeleteSpriteInstance()
	{
		RENDER_MANAGER->GetRenderItemManager().RemoveRenderItem(mp_sprite);
	}

}
