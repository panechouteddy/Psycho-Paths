#include "pch.h"
#include "UISprintBar.h"
#include "constants.h"

#include "UIObject.h"

void UISprintBar::OnInit()
{
	SetBehavior();

	XMFLOAT2 w = WINDOW->GetSize(true);

	m_barPos = { 150.f, w.y - 75.f };

	mp_sprintBar = GetScene()->CreateGameObject<UIObject>();
	{
		mp_sprintBar->CreateSprite(m_barPos, m_barSize, {1, 1, 1, 0.75f}, - 1);
		mp_sprintBar->mp_sprite->SetDrawLayer(5);
	}

	mp_sprintBarSprite = GetScene()->CreateGameObject<UIObject>();
	{
		mp_sprintBarSprite->CreateSprite(m_barPos, { m_barSize.x * 1.1f , m_barSize.y * 2.9f }, { 1, 1, 1 ,0.5f }, (int)TagTexture::_SprintBar);
		mp_sprintBarSprite->mp_sprite->SetDrawLayer(5);
	}
}

void UISprintBar::OnStart()
{

}

void UISprintBar::OnUpdate()
{

}

void UISprintBar::OnCollision(u32 self, u32 other, const CollisionInfo& collisionInfo)
{
}

void UISprintBar::UpdateBar(float currentSprintLimit)
{
	XMFLOAT2 w = WINDOW->GetSize(true);

	float sprintRatio = currentSprintLimit / PLAYER_RUN_LIMIT;
	XMFLOAT2 scale = { m_barSize.x * sprintRatio, m_barSize.y };
	mp_sprintBar->SetUiScale(scale);

	float offset = m_barSize.x * (1.f - sprintRatio);
	float newPosX = (m_barPos.x * 2.f - offset) * 0.5f;
	mp_sprintBar->SetUiPosition({ newPosX, w.y * 0.9f });
}

void UISprintBar::SetActiveUI(bool state)
{
	mp_sprintBarSprite->SetActive(state);
	mp_sprintBar->SetActive(state);
}
