#pragma once
#include "GameObject.h"

class TriggerFinishClimb : public GameObject
{
public:
	void OnInit() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnCollision(u32 self, u32 other, const CollisionInfo& collisionInfo) override;
};

