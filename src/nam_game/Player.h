#pragma once
#include "GameObject.h"
#include "Inventory.h"

class Flashlight;
class UISprintBar;

class FootstepNoise;

class Player : public GameObject
{
public : 
	enum States
	{
		_IDLE,
		_WALK,
		_RUN,
		_JUMP,
		_CROUCH,
		_SLIDE,
		_CLIMB,
		_HIDE,
		_DEATH
	};

	StatesManager m_stateManager;

private:
	XMFLOAT3 m_size = {0, 0, 0};

	Flashlight* mp_flashlight = nullptr;
	GameObject* mp_ambientLight = nullptr;

	UISprintBar* mp_sprintBar = nullptr;
	FootstepNoise* mp_footstepNoise = nullptr;

	Inventory m_inventory;

	bool m_pursuitMode = false;

	bool m_isGrounded = false;
	bool m_isClimbing = false;
	bool m_doSlide = false;

	float m_speed = -1;
	float m_sprintLimit = -1;

	Timer m_slideTimer;
	Timer m_timeBeforeSlide;
	Timer m_timerBeforeClimb;

	XMFLOAT3 m_localMoveDir = { 0, 0, 0 };

	bool m_isPaused = false;
	DirectX::XMFLOAT3 m_camYPR = {};

	int m_batteryCount = 0;
	XMFLOAT3 m_currentClimbNormal;
	bool m_enterCollideClimb = false;

	bool m_hasFlashlight = false;
public:
	void OnInit() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnCollision(u32 self, u32 other, const CollisionInfo& collisionInfo) override;
	void HandleInputs();
	void OnDestroy() override;
	void CheckStates(float dt);

	void HandleCamera(GamePad* p_0);

	//States
	void SwitchHideState();
	//Move
	void Move(float speed);
	void Jump(float _impulse);
	void Crouch();
	void StandUp();
	void Climb();
	void DownClimb();

	const int GetBatteryCount() const;
	void IncreaseBatteryCount();
	void DecreaseBatteryCount();

	void SetActiveUI(bool state);

	Inventory& GetInventory();

	void SetPursuitMode(bool state);
	void SetHasFlashlight(bool state);
	const bool HasFlashlight() const;

private:
	void UpdateChilds();

	void HandlePausing();
	void HandleInventory();
};

