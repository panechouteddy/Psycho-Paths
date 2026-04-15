#pragma once
#include <GameObject.h>

class SavePoint : public GameObject
{
public:
	void OnInit() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnEnd() override;

	void OnCollision(u32 self, u32 other, const CollisionInfo& collisionInfo) override;
	void SetColliderSize(XMFLOAT3 size);
};

