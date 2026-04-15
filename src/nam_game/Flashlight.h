#pragma once
#include "GameObject.h"

class UIBatteryCharge;
class UIObject;

class Flashlight : public GameObject
{
public: 
	enum StatesFlashlight
	{
		_ON,
		_OFF,
		_RELOAD,
		_STROKING,
		_BATTERY_DOWN,
	};

private:
	StatesManager m_stateManager;

	UIBatteryCharge* mp_batteryCharge = nullptr;

	Timer m_batteryLevel;
	Timer m_strokingTimer;
	Timer m_activateTimer;
	Timer m_desactivateTimer;
	Timer m_replaceTimer;

	int m_countStroking = -1;

	float m_maxIntensity = -1;
	bool m_playerSwitch = false;

public:
	void OnInit() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnCollision(u32 self, u32 other, const CollisionInfo& collisionInfo) override;
	void CheckStates(float dt);

	void ReplaceBattery();
	void StartReplaceTimer();
	void TurnOnLight(LightComponent lc);
	void TurnOffLight(LightComponent lc);
	void SwitchLight();
	void StrokingLight(float dt);

	void SetActiveUI(bool state);
	void SetFlashLightOff();
};

