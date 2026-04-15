#include "pch.h"
#include "PhysicSystem.h"
#include "TransformComponent.h"
#include "PhysicComponent.h"
#include "App.h"
#include "DebugLog.h"

using namespace DirectX;

namespace nam
{
	void PhysicSystem::Update(Ecs& ecs)
	{
		float dt = App::Get()->GetChrono().GetScaledDeltaTime();
		float gravity = dt * m_gravity;

		ecs.ForEach<TransformComponent, PhysicComponent>([&](uint32_t entity, TransformComponent& transform, PhysicComponent& physic) {
			transform.UpdateWorldData();

			if (physic.m_useGravity) {
				physic.m_velocity.y -= gravity * physic.m_gravityScale;
			}

			if (physic.m_velocity.x != 0 || physic.m_velocity.y != 0 || physic.m_velocity.z != 0)
			{
				if (physic.m_isKinematic)
					return;

				XMFLOAT3 worldPosition = transform.GetWorldPosition();
				XMVECTOR pos = XMLoadFloat3(&worldPosition);
				XMVECTOR vel = XMLoadFloat3(&physic.m_velocity);

				pos = XMVectorAdd(pos, XMVectorMultiply(vel, XMVectorReplicate(dt)));

				XMFLOAT3 newPos;
				XMStoreFloat3(&newPos, pos);
				transform.SetWorldPosition(newPos);
			}
		});
	}
}
