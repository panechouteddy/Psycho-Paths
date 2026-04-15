#pragma once
#include "GameObject.h"

class UIObject;

class UISprintBar : public GameObject
{
private:
	XMFLOAT2 m_barPos = { 0, 0 };
	XMFLOAT2 m_barSize = { 200.f, 20.f };

	UIObject* mp_sprintBarSprite = nullptr;
	UIObject* mp_sprintBar = nullptr;

public:
	void OnInit() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnCollision(u32 self, u32 other, const CollisionInfo& collisionInfo) override;

	void UpdateBar(float currentSprintLimit);

	void SetActiveUI(bool state);
};

