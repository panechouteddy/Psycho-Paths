#include "pch.h"
#include "TextRendererComponent.h"
#include <RenderManager.h>

namespace nam
{
	Text* TextRendererComponent::CreateTextInstance()
	{
		mp_text = RENDER_MANAGER->GetRenderItemManager().CreateRenderItem<Text>();

		return mp_text;
	}

	void TextRendererComponent::SetTextInstance(Text* p_text)
	{
		if (p_text == nullptr)
		{
			_ASSERT(false && "You setted a nullptr Text");
			return;
		}

		if (p_text->GetID() == INVALID_ID)
		{
			_ASSERT(false && "YOU CAN'T CREATE RAW SPRITES INSTANCES, YOU NEED TO CREATE A TEXT\nWITH THE RenderItemManager class : Use app->CreateEmptyText()");
			return;
		}

		mp_text = p_text;
	}

	void TextRendererComponent::DeleteTextInstance()
	{
		RENDER_MANAGER->GetRenderItemManager().RemoveRenderItem(mp_text);
	}
}