#pragma once
#include "UISprintBar.h"
#include "constants.h"

class UIObject;

class UIBatteryCharge : public nam::GameObject
{
private:
	XMFLOAT2 m_uiPos = { 0, 0 };
	XMFLOAT2 m_batterySize = { 100.f, 40.f };

	UIObject* mp_batterySprite = nullptr;
	UIObject* mp_batteryCharge = nullptr;
	UIObject* mp_batteryCountText = nullptr;

public:
	void OnInit() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnCollision(u32 self, u32 other, const CollisionInfo& collisionInfo) override;
	void UpdateBatteryCharge(float currentCharge, float maxCharge);
	void ResetUI();

	void SetActiveUI(bool state);
};

