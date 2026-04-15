#pragma once
#include "DirectXMath.h"
#include <DirectXCollision.h>

namespace nam
{
	struct TransformComponent;
	struct MeshRendererComponent;
	struct CollisionInfo;

	struct SphereColliderComponent
	{
		int m_collisionTag = -1;
		u32 m_updateId = 0;
		DirectX::BoundingSphere m_box;
		bool m_noIntersectionPush = false;
		std::function<void(u32 self, u32 other, const CollisionInfo& collisionInfo)> OnCollision;
		void UpdateSphereBox(TransformComponent& transform, const MeshRendererComponent& meshRenderer, u32 updateId);
	};
}
