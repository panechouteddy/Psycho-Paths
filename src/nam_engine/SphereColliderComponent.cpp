#include "pch.h"
#include "SphereColliderComponent.h"

using namespace DirectX;

namespace nam
{
	void SphereColliderComponent::UpdateSphereBox(TransformComponent& transform, const MeshRendererComponent& meshRenderer, u32 updateId)
	{
		if (updateId == m_updateId)
			return;

		const Mesh* p_mesh = meshRenderer.mp_mesh;
		XMFLOAT3 worldPosition = transform.GetWorldPosition();
		XMFLOAT3 worldCenter;
		XMStoreFloat3(&worldCenter,
			XMVectorAdd(
				XMLoadFloat3(&p_mesh->m_center),
				XMLoadFloat3(&worldPosition)
			));

		XMFLOAT3 scaledExtend;
		XMFLOAT3 worldScale = transform.GetWorldScale();
		XMStoreFloat3(
			&scaledExtend,
			XMVectorMultiply(
				XMLoadFloat3(&p_mesh->m_extend),
				XMLoadFloat3(&worldScale)
			)
		);

		float radius = meshRenderer.mp_mesh->m_radius;

		if (radius == -1)
		{
			XMVECTOR vExtents = XMLoadFloat3(&scaledExtend);
			radius = XMVectorGetX(XMVector3Length(vExtents));
		}

		m_box = BoundingSphere(worldCenter, radius);
		m_updateId = updateId;
	}
}