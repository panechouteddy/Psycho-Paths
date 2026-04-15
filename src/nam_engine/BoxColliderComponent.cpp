#include "pch.h"
#include "BoxColliderComponent.h"
#include "TransformComponent.h"
#include "MeshRendererComponent.h"

using namespace DirectX;

namespace nam
{
	void BoxColliderComponent::UpdateObbBox(TransformComponent& transform, const MeshRendererComponent& meshRenderer, u32 updateId)
	{
		if (updateId == m_updateId)
			return;

		const Mesh* p_mesh = meshRenderer.mp_mesh;

		XMFLOAT3 worldPosition = transform.GetWorldPosition();
		XMFLOAT3 worldCenter = {};

		XMFLOAT3 localCenter = {};

		if (m_useMeshCenter)
			localCenter = p_mesh->m_center;
		else
			localCenter = m_center;

		XMStoreFloat3(&worldCenter,
			XMVectorAdd(
				XMLoadFloat3(&localCenter),
				XMLoadFloat3(&worldPosition)
			));

		XMFLOAT3 scaledExtend = {};

		XMFLOAT3 localExtend = {};

		if (m_useManualBox)
			localExtend = m_extend;
		else
			localExtend = p_mesh->m_extend;

		XMFLOAT3 worldScale = transform.GetWorldScale();
		XMStoreFloat3(
			&scaledExtend,
			XMVectorMultiply(
				XMLoadFloat3(&localExtend),
				XMLoadFloat3(&worldScale)
			)
		);

		m_box = BoundingOrientedBox(worldCenter, scaledExtend, transform.GetWorldRotation());
		m_updateId = updateId;
	}

	void BoxColliderComponent::UpdateObbBox(TransformComponent& transform, u32 updateId)
	{
		if (updateId == m_updateId)
			return;

		XMFLOAT3 worldPosition = transform.GetWorldPosition();
		XMFLOAT3 worldCenter;
		XMStoreFloat3(&worldCenter,
			XMVectorAdd(
				XMLoadFloat3(&m_center),
				XMLoadFloat3(&worldPosition)
			));

		XMFLOAT3 scaledExtend;
		XMFLOAT3 worldScale = transform.GetWorldScale();
		XMStoreFloat3(
			&scaledExtend,
			XMVectorMultiply(
				XMLoadFloat3(&m_extend),
				XMLoadFloat3(&worldScale)
			)
		);

		m_box = BoundingOrientedBox(worldCenter, scaledExtend, transform.GetWorldRotation());
		m_updateId = updateId;
	}

	void BoxColliderComponent::CreateManualBox(DirectX::XMFLOAT3 boxSize, bool useMeshCenter, DirectX::XMFLOAT3 center)
	{
		m_useManualBox = true;

		m_boxSize = boxSize;
		m_useMeshCenter = useMeshCenter;
		m_center = center;

		m_extend.x = boxSize.x * 0.5f;
		m_extend.y = boxSize.y * 0.5f;
		m_extend.z = boxSize.z * 0.5f;
	}
}
