#include "pch.h"
#include "Flashlight.h"
#include "constants.h"
#include "UIBatteryCharge.h"

#include "UIObject.h"

void Flashlight::OnInit()
{
	SetBehavior();
	SetTag((int)TagObject::_Flashlight);
	m_stateManager.Init({ StatesFlashlight::_ON, StatesFlashlight::_OFF, StatesFlashlight::_RELOAD, StatesFlashlight::_BATTERY_DOWN, StatesFlashlight::_STROKING});
	m_stateManager.SetExclusiveActiveState(StatesFlashlight::_OFF);

	m_maxIntensity = MAX_INTENSITY;

	LightComponent lc;
	Light* pl = lc.CreateLightInstance();
	pl->SetToSpotLight(m_maxIntensity, { 0.f, 1.f, 0.f }, 20.f, { 0, 0, 0 }
	, cosf(6.f * DEGREE_TO_RADIAN), cosf(20.f * DEGREE_TO_RADIAN), { 1.f, 1.f, 0.85f });
	AddComponent(lc);

	mp_batteryCharge = GetScene()->CreateGameObject<UIBatteryCharge>();

	m_batteryLevel.Init(MAX_BATTERY, true, true);
	m_strokingTimer.Init(BATTERY_PAUSE_BETWEEN_STROKING, false, true);
	m_desactivateTimer.Init(0.1f, false, true);
	m_activateTimer.Init(0.1f, false, true);
	m_replaceTimer.Init(2.f, false, true);

	m_isActive = false;
	m_playerSwitch = false;
}

void Flashlight::OnStart()
{
	m_stateManager.SetExclusiveActiveState(StatesFlashlight::_OFF);
	m_batteryLevel.ResetProgress();
	m_strokingTimer.ResetProgress();
	m_desactivateTimer.ResetProgress();
	m_activateTimer.ResetProgress();
	m_replaceTimer.ResetProgress();
}

void Flashlight::OnUpdate()
{
	AppChrono& chrono = App::Get()->GetChrono();
	float dt = chrono.GetScaledDeltaTime();

	LightComponent lc = GetComponent<LightComponent>();

	m_batteryLevel.Update(dt);
	m_replaceTimer.Update(dt);
	
	CheckStates(dt);
	mp_batteryCharge->UpdateBatteryCharge(m_batteryLevel.GetProgress(), m_batteryLevel.GetTargetTime());

	if (m_batteryLevel.IsTargetReached() == true)
	{
		m_stateManager.SetExclusiveActiveState(StatesFlashlight::_BATTERY_DOWN);
		m_batteryLevel.Pause();
	}

	float timeRemaining = (m_batteryLevel.GetProgress() / m_batteryLevel.GetTargetTime());
	if (timeRemaining <= 0.3f && m_playerSwitch == false)
	{
		m_stateManager.ActivateState(StatesFlashlight::_STROKING);
	}

}

void Flashlight::OnCollision(u32 self, u32 other, const CollisionInfo& collisionInfo)
{
}

void Flashlight::CheckStates(float dt)
{
	LightComponent lc = GetComponent<LightComponent>();

	if (m_stateManager.IsActiveState(StatesFlashlight::_ON) == true)
		TurnOnLight(lc);

	if (m_stateManager.IsActiveState(StatesFlashlight::_OFF) == true)
		TurnOffLight(lc);
	
	if (m_stateManager.IsActiveState(StatesFlashlight::_RELOAD) == true)
		ReplaceBattery();
	
	if (m_stateManager.IsActiveState(StatesFlashlight::_STROKING) == true)
		StrokingLight(dt);
	
	if (m_stateManager.IsActiveState(StatesFlashlight::_BATTERY_DOWN) == true)
		TurnOffLight(lc);
}

void Flashlight::ReplaceBattery()
{
	if (m_replaceTimer.IsTargetReached() == true)
	{
		m_playerSwitch = false;
		m_replaceTimer.ResetProgress();
		m_replaceTimer.Pause();
		m_batteryLevel.ResetProgress();
		m_batteryLevel.Resume();
		m_stateManager.SetExclusiveActiveState(StatesFlashlight::_ON);
		mp_batteryCharge->ResetUI();
	}
}

void Flashlight::StartReplaceTimer()
{
	m_replaceTimer.Resume();
	m_batteryLevel.Pause();
	m_stateManager.ActivateState(StatesFlashlight::_OFF);
	m_stateManager.ActivateState(StatesFlashlight::_RELOAD);
	AudioManager::PlaySound("FlashReload");

	m_stateManager.DesactivateState(StatesFlashlight::_ON);
}

void Flashlight::TurnOnLight(LightComponent lc)
{
	m_batteryLevel.Resume();
	lc.mp_light->m_intensity = m_maxIntensity;
}

void Flashlight::TurnOffLight(LightComponent lc)
{
	m_batteryLevel.Pause();
	lc.mp_light->m_intensity = 0.f;
}

void Flashlight::SwitchLight()
{
	if (m_stateManager.IsActiveState(StatesFlashlight::_BATTERY_DOWN) == true
		|| m_stateManager.IsActiveState(StatesFlashlight::_RELOAD) == true)
	{
		return;
	}

	if (m_stateManager.IsActiveState(StatesFlashlight::_ON) == true)
	{
		AudioManager::PlaySound("Flashlight", 1.f);
		m_stateManager.ActivateState(StatesFlashlight::_OFF);
		m_stateManager.DesactivateState(StatesFlashlight::_ON);
		if (m_stateManager.IsActiveState(StatesFlashlight::_STROKING) == true)
		{
			m_playerSwitch = true;
			m_stateManager.DesactivateState(StatesFlashlight::_STROKING);
		}
	}
	else if (m_stateManager.IsActiveState(StatesFlashlight::_OFF) == true)
	{
		AudioManager::PlaySound("Flashlight", 1.f);
		m_stateManager.ActivateState(StatesFlashlight::_ON);
		m_stateManager.DesactivateState(StatesFlashlight::_OFF);
		float timeRemaining = (m_batteryLevel.GetProgress() / m_batteryLevel.GetTargetTime());
		if (timeRemaining <= 0.3f)
		{
			m_playerSwitch = false;
			m_stateManager.ActivateState(StatesFlashlight::_STROKING);
		}
	}
}

void Flashlight::StrokingLight(float dt)
{
	LightComponent lc = GetComponent<LightComponent>();
	m_strokingTimer.Resume();
	m_strokingTimer.Update(dt);
	m_activateTimer.Update(dt);
	m_desactivateTimer.Update(dt);
	if (m_strokingTimer.IsTargetReached() == true)
	{
		m_strokingTimer.ResetProgress();
		m_desactivateTimer.ResetProgress();
		m_activateTimer.ResetProgress();
		m_desactivateTimer.Resume();
		m_activateTimer.Pause();
		m_countStroking = 0;
	}
	if (m_strokingTimer.IsTargetReached() == false)
	{
		if (m_desactivateTimer.IsTargetReached() == true)
		{
			m_desactivateTimer.ResetProgress();
			m_desactivateTimer.Pause();
			m_activateTimer.Resume();
			m_stateManager.ActivateState(StatesFlashlight::_OFF);
			m_stateManager.DesactivateState(StatesFlashlight::_ON);
			m_countStroking++;
		}
		if (m_activateTimer.IsTargetReached() == true)
		{	
			m_activateTimer.ResetProgress();
			m_activateTimer.Pause();
			m_desactivateTimer.Resume();
			m_stateManager.ActivateState(StatesFlashlight::_ON);
			m_stateManager.DesactivateState(StatesFlashlight::_OFF);
			m_countStroking++;
		}
	}
	if (m_countStroking >= 4)
	{
		m_activateTimer.ResetProgress();
		m_desactivateTimer.ResetProgress();
		m_desactivateTimer.Pause();
		m_activateTimer.Pause(); 
	}
}

void Flashlight::SetActiveUI(bool state)
{
	mp_batteryCharge->SetActiveUI(state);
}

void Flashlight::SetFlashLightOff()
{
	m_stateManager.SetExclusiveActiveState(StatesFlashlight::_OFF);
}


