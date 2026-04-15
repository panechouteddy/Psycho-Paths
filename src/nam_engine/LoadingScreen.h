#pragma once
#include "GameObject.h"
#include <BaseTypes.h>
#include <Timer.h>

namespace nam
{
	class LoadingScreen : public GameObject
	{
	public:
		bool m_spawn = true;

		Timer m_waitingTimer = {};
		Timer m_fadingTimer = {};
		float m_fadingSpeed = 0.25f;

	public:
		void OnInit() override;
		void OnStart() override;
		void OnUpdate() override;
		void OnCollision(u32 self, u32 other, const CollisionInfo& collisionInfo) override;
		void OnDestroy() override;

		void Activate(float minTimeToWait);
	};
}

