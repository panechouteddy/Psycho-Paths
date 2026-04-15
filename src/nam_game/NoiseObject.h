#pragma once
#include <GameObject.h>

struct NoiseComponent;

class NoiseObject : public nam::GameObject
{
public:
	void SetNoiseComponent(std::string soundName, float soundScale, float soundRange);
	void OnCollision(u32 self, u32 other, const CollisionInfo& collisionInfo) override;
	void TriggerNoise();
};

