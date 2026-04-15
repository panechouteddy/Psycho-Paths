#include "pch.h"
#include "UiObject.h"

void UIObject::OnInit()
{
	SetBehavior();
}

void UIObject::OnStart()
{

}

void UIObject::OnUpdate()
{
	XMFLOAT2 windowStartSize = WINDOW->GetSize(true);
	XMFLOAT2 windowSize = WINDOW->GetSize(false);

	if (m_forceReplacement == false)
	{
		if ((int)m_currentWinSize.x == (int)windowSize.x && (int)m_currentWinSize.y == (int)windowSize.y)
			return;
	}
	else
		m_forceReplacement = false;

	m_currentWinSize = windowSize;
	XMFLOAT2 ratio = { m_currentWinSize.x / windowStartSize.x, m_currentWinSize.y / windowStartSize.y };

	if (mp_sprite)
	{
		mp_sprite->BuildRect({ m_defaultScale.x * ratio.x, m_defaultScale.y * ratio.y }, mp_sprite->m_color, true);
		mp_transform->SetWorldPosition({ m_defaultPos.x * ratio.x ,m_defaultPos.y * ratio.y ,0 });
	}
	else if(mp_text)
	{
		mp_text->SetCharacterSize({ m_defaultScale.x * ratio.x, m_defaultScale.y * ratio.y });

		XMFLOAT2 offset = mp_text->GetOffsetsToCenter();
		mp_transform->SetWorldPosition({ m_defaultPos.x * ratio.x  - offset.x, m_defaultPos.y * ratio.y - offset.y ,0 });
	}
}

void UIObject::CreateSprite(XMFLOAT2 pos, XMFLOAT2 size, XMFLOAT4 color, int textureTag)
{
	if (mp_sprite || mp_text)
	{
		_ASSERT(false && "Already Init");
		return;
	}

	SpriteRendererComponent spriteComp;
	mp_sprite = spriteComp.CreateSpriteInstance();
	mp_sprite->SetTexture(textureTag);

	m_defaultPos = pos;
	m_defaultScale = size;

	mp_sprite->BuildRect(size, color);
	mp_sprite->SetToDynamic(4, 6);

	AddComponent(spriteComp);
}

void UIObject::CreateText(XMFLOAT2 pos, std::string txt, float scale, XMFLOAT4 color, int textureTag, std::string fontName)
{
	if (mp_sprite || mp_text)
	{
		_ASSERT(false && "Already Init");
		return;
	}

	m_name = txt;

	m_defaultPos = pos;
	m_defaultScale = { scale, scale };

	TextRendererComponent textComp;
	mp_text = textComp.CreateTextInstance();
	mp_text->SetDatas(m_defaultScale, color, textureTag, fontName);
	mp_text->SetText(txt);
	mp_text->SetToDynamic(MAX_TEXT_VERTICES, MAX_TEXT_INDICES);
	AddComponent(textComp);
}

void UIObject::SetUiPosition(XMFLOAT2 pos)
{
	m_forceReplacement = true;
	m_defaultPos = pos;
}

void UIObject::TranslateUiPosition(XMFLOAT2 t)
{
	m_forceReplacement = true;
	m_defaultPos.x += t.x;
	m_defaultPos.y += t.y;
}

void UIObject::SetUiScale(XMFLOAT2 scale)
{
	m_forceReplacement = true;
	m_defaultScale = scale;
}
