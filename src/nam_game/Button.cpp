#include "pch.h"
#include "Button.h"

#include <Sprite.h>
#include <SpriteRendererComponent.h>
#include <ButtonComponent.h>

void Button::OnInit()
{
	SetBehavior();
	SetTag((int)TagObject::_Button);
}

void Button::OnStart()
{
}

void Button::OnUpdate()
{
	Window& win = APP->GetWindow();

	int start_width = WINDOW->m_startWidth;
	int start_height = WINDOW->m_startHeight;

	int win_width = WINDOW->m_clientWidth;
	int win_height = WINDOW->m_clientHeight;

	if (m_forceReplacement == false)
	{
		if (m_currWindowWidth == win_width && m_currWindowHeight == win_height)
			return;
	}
	else
		m_forceReplacement = false;

	m_currWindowWidth = win_width;
	m_currWindowHeight = win_height;

	XMFLOAT2 half = { win_width * 0.5f, win_height * 0.5f };
	XMFLOAT2 ratio = { (float)win_width / (float)start_width, (float)win_height / (float)start_height};

	XMFLOAT3 newPos = { m_startPos.x * ratio.x, m_startPos.y * ratio.y, 0.f };
	XMFLOAT2 newScale = { m_size.x * ratio.x, m_size.y * ratio.y };

	mp_sprite->BuildRect(newScale, m_color, true);
	mp_transform->SetWorldPosition(newPos);

	if (mp_textGO == nullptr)
		return;

	if (mp_textGO->HasComponent<TextRendererComponent>() == false)
		return;

	Text* p_text = mp_textGO->GetComponent<TextRendererComponent>().mp_text;
	p_text->SetCharacterSize({ ratio.x * m_textScale.x, ratio.y * m_textScale.y });

	XMFLOAT2 offsetsText = p_text->GetOffsetsToCenter();
	newPos.x -= offsetsText.x;
	newPos.y -= offsetsText.y;
	mp_textGO->mp_transform->SetWorldPosition(newPos);
}

void Button::OnDestroy()
{
	mp_textGO->Destroy();
}

void Button::SetProperties(XMFLOAT2 pos, XMFLOAT2 dimensions, XMFLOAT4 color, int textureIdle, int texturePressed, std::function<void()> onClick)
{
	if (HasComponent<SpriteRendererComponent>())
		return;

	m_defaultColor = color;
	m_color = color;
	m_startPos = pos;
	m_size = dimensions;
	m_textureIdle = textureIdle;
	m_texturePressed = texturePressed;
	m_onClick = onClick;

	SpriteRendererComponent spriteComp;
	mp_sprite = spriteComp.CreateSpriteInstance();
	mp_sprite->BuildRect({ dimensions.x, dimensions.y }, m_color);
	mp_sprite->SetToDynamic(4, 6);
	mp_sprite->SetDrawLayer(1);
	mp_sprite->SetTexture(m_textureIdle);
	AddComponent(spriteComp);

	ButtonComponent buttonComp;
	buttonComp.OnHovered = [this]()
	{	
		m_hovered = true;
		SaturateColor(2.f);
		mp_sprite->SetTexture(m_texturePressed);
		AudioManager::PlaySound("ButtonHover", 1.f);
	};
	buttonComp.OnLeft = [this]()
	{
		m_hovered = false;
		SaturateColor(1.f);
		mp_sprite->SetTexture(m_textureIdle);
		AudioManager::PlaySound("ButtonRelease", 1.f);
	};
	buttonComp.OnClick = [this]() { m_hovered = false; m_onClick(); };
	AddComponent(buttonComp);
}

void Button::AddText(std::string txt, XMFLOAT2 scale, XMFLOAT4 color, int drawLayerText)
{
	if (mp_textGO != nullptr)
		return;

	mp_sprite->SetDrawLayer(drawLayerText + 1);

	m_textScale = scale;

	mp_textGO = GetScene()->CreateGameObject<GameObject>();
	{
		mp_textGO->AddComponent(TextRendererComponent());

		TextRendererComponent& textComp = mp_textGO->GetComponent<TextRendererComponent>();
		Text* p_text = textComp.CreateTextInstance();
		p_text->SetDatas({ 1.f, 1.f }, color, (int)TagTexture::_FontSen, "Sen");
		p_text->SetText(txt);
		p_text->SetDrawLayer(drawLayerText);
		p_text->SetToDynamic(MAX_TEXT_VERTICES, MAX_TEXT_INDICES);
	
	}
}

void Button::SetColor(DirectX::XMFLOAT4 color)
{
	if (mp_sprite == nullptr)
		return;

	m_defaultColor = color;
	m_color = color;
	mp_sprite->SetColor(m_color);
}

void Button::SetActive(bool state)
{
	GameObject::SetActive(state);

	if(mp_textGO)
		mp_textGO->SetActive(state);
}

void Button::SetUiPosition(XMFLOAT2 pos)
{
	m_forceReplacement = true;
	m_startPos = pos;
}
void Button::TranslateUiPosition(XMFLOAT2 t)
{
	m_forceReplacement = true;
	m_startPos.x += t.x;
	m_startPos.y += t.y;
}

void Button::ChangeText(std::string txt)
{
	m_forceReplacement = true;
	mp_textGO->GetComponent<TextRendererComponent>().mp_text->SetText(txt);
}

void Button::SaturateColor(float scale)
{
	XMVECTOR color = XMLoadFloat4(&m_defaultColor);

	color *= scale;
	XMStoreFloat4(&m_color, color);
	m_color.w = m_defaultColor.w;

	mp_sprite->SetColor(m_color);
}