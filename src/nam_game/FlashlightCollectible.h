#pragma once
#include "Collectible.h"

class FlashlightCollectible : public Collectible
{
public:
	void OnInit() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnCollision(u32 self, u32 other, const CollisionInfo& collisionInfo) override;
	void OnUse() override;
};

