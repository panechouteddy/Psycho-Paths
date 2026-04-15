#pragma once
#include <DirectXMath.h>
#include "Common/MathHelper.h"

namespace nam
{
	struct CameraComponent
	{
		float m_fovY = -1;
		float m_aspectRatio = -1;
		float m_nearPlane = -1;
		float m_farPlane = -1;

		//For Frustum culling
		DirectX::XMFLOAT4 m_planes[6];

		DirectX::XMFLOAT4X4 m_view = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 m_proj = MathHelper::Identity4x4();

		CameraComponent(float fovY = DirectX::XM_PIDIV4, float nearPlane = 0.01f, float farPlane = 200.f);
		void UpdateAspectRatio(int clientWidth = -1, int clientHeight = -1);

		void SetFov(float fovY);
		void SetNearPlane(float nearPlane);
		void SetFarPlane(float farPlane);

		void SetProperties(float fovY, float nearPlane, float farPlane);

		void UpdateFrustum();

		bool IsVisibleInRadius(XMFLOAT3 center, float radius);

	private:
		void CalculateProj();

		void ComputePlanes();
		void NormalizePlanes();
	};
}
