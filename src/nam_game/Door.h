#pragma once
#include <GameObject.h>

struct Dimensions
{
	float width = 0, height = 0, depth = 0;
};

class Door : public nam::GameObject
{
	Dimensions m_dims;

	XMFLOAT3 m_pivotPoint = {};

	bool m_isClosed = true;
	bool m_isOnAnimation = false;
	bool m_canOpen = true;
	bool m_canOpenFirst = false;

	//If false the pivot is on the left
	bool m_rightPivot = false;

	//Invert the direction where the door rotates
	bool m_inner = false;

	float m_angle = 0.f;
	float m_maxAngle = XM_PIDIV2;

	Timer m_animTimer;

public:
	void OnInit() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnEnd() override;

	void OnCollision(u32 self, u32 other, const CollisionInfo& collisionInfo) override;
	void OnDestroy() override;

	void SetProperties(Dimensions dimensions, bool rightPivot, bool inner, float maxAngle, float animDuration, bool canOpen);
	void CanOpen(bool canOpen);

	void ToggleDoor();
private:
	void HandleAnimation();
};

