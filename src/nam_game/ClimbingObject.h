#pragma once
#include "GameObject.h"

class TriggerStartClimb;
class TriggerFinishClimb;

class ClimbingObject : public GameObject
{
private :

	TriggerStartClimb* mp_triggerStart = nullptr;
	TriggerFinishClimb* mp_triggerFinish = nullptr;

	XMFLOAT3 m_size;
public:
	void OnInit() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnCollision(u32 self, u32 other, const CollisionInfo& collisionInfo) override;
	XMFLOAT3 GetSize();
};

