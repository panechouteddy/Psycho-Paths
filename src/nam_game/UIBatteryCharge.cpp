#include "pch.h"
#include "UIBatteryCharge.h"

#include "UIObject.h"

void UIBatteryCharge::OnInit()
{
	SetBehavior();

	XMFLOAT2 winStartSize = WINDOW->GetSize(true);
	m_uiPos = { winStartSize.x - 125.f, winStartSize.y - 75.f };

	mp_batterySprite = GetScene()->CreateGameObject<UIObject>();
	{
		mp_batterySprite->CreateSprite({ 0, 0 }, { m_batterySize.x * 1.4f, m_batterySize.y * 1.8f }, { 1, 1, 1, 0.5f }, (int)TagTexture::_BatteryCharge);
		mp_batterySprite->mp_sprite->SetDrawLayer(2);
	}

	mp_batteryCharge = GetScene()->CreateGameObject<UIObject>();
	{
		mp_batteryCharge->CreateSprite({ 0, 0 }, m_batterySize, {1, 1, 1, 0.75f}, - 1);
		mp_batteryCharge->mp_sprite->SetDrawLayer(1);
	}

	mp_batteryCountText = GetScene()->CreateGameObject<UIObject>();
	{
		XMFLOAT2 winSize = WINDOW->GetSize(true);

		mp_batteryCountText->CreateText({ winSize.x - 125.f, winSize.y - 125.f }, " ", 0.75f, {1, 1, 1, 0.75f});
		mp_batteryCountText->mp_text->SetDrawLayer(0);
	}	
}

void UIBatteryCharge::OnStart()
{
	ResetUI();
}

void UIBatteryCharge::OnUpdate()
{
	Player* p_player = (Player*)GetScene()->GetFirstGameObjectWithTag((int)TagObject::_Player);

	if (p_player == nullptr)
		return;

	mp_batteryCountText->mp_text->SetText("Recharges : " + std::to_string(p_player->GetBatteryCount()));
}

void UIBatteryCharge::OnCollision(u32 self, u32 other, const CollisionInfo& collisionInfo)
{
}

void UIBatteryCharge::UpdateBatteryCharge(float currentCharge, float maxCharge)
{
	float ratio = (currentCharge / maxCharge);
	float scaleRounded = std::clamp((int)(ratio * 10 + 1) / 10.f, 0.f, 1.f);

	float offset = (m_batterySize.x * (1.f - scaleRounded));
	float newPosX = (m_uiPos.x - offset * 0.5f);

	mp_batteryCharge->SetUiScale({ m_batterySize.x * scaleRounded, m_batterySize.y });
	mp_batteryCharge->SetUiPosition({ newPosX, m_uiPos.y });
}

void UIBatteryCharge::ResetUI()
{
	XMFLOAT2 winStartSize = WINDOW->GetSize(true);
	mp_batteryCharge->SetUiScale(m_batterySize);
	mp_batterySprite->SetUiPosition({ m_uiPos.x + 5.f, m_uiPos.y });
	mp_batteryCharge->SetUiPosition(m_uiPos);
}

void UIBatteryCharge::SetActiveUI(bool state)
{
	mp_batterySprite->SetActive(state);
	mp_batteryCharge->SetActive(state);
	mp_batteryCountText->SetActive(state);
}