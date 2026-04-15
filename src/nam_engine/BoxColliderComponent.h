#pragma once
#include "DirectXMath.h"
#include <DirectXCollision.h>

namespace nam
{
	struct TransformComponent;
	struct MeshRendererComponent;
	struct CollisionInfo;

	struct BoxColliderComponent
	{
	public:
		int m_collisionTag = -1;
		u32 m_updateId = 0;
		DirectX::BoundingOrientedBox m_box;
		bool m_noIntersectionPush = false;
		std::function<void(u32 self, u32 other, const CollisionInfo& collisionInfo)> OnCollision;

	private:
		bool m_useManualBox = false;
		bool m_useMeshCenter = true;
		DirectX::XMFLOAT3 m_boxSize = {};
		DirectX::XMFLOAT3 m_center = {};
		DirectX::XMFLOAT3 m_extend = {};

	public:
		void UpdateObbBox(TransformComponent& transform, const MeshRendererComponent& meshRenderer, u32 updateId);
		void UpdateObbBox(TransformComponent& transform, u32 updateId);

		void CreateManualBox(DirectX::XMFLOAT3 boxSize, bool useMeshCenter = true, DirectX::XMFLOAT3 center = { 0, 0, 0 });
	};
}
