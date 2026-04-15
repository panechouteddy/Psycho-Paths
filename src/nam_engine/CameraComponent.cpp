#include "pch.h"
#include "CameraComponent.h"
#include "App.h"
#include "Window.h"

namespace nam
{
	CameraComponent::CameraComponent(float fovY, float nearPlane, float farPlane)
	{
		m_fovY = fovY;
		m_nearPlane = nearPlane;
		m_farPlane = farPlane;

		UpdateAspectRatio();
	}
	void CameraComponent::UpdateAspectRatio(int clientWidth, int clientHeight)
	{
		if (clientWidth == -1 || clientHeight == -1 || clientHeight == 0)
		{
			Window& win = App::Get()->GetWindow();
			m_aspectRatio = (float)win.m_clientWidth / (float)win.m_clientHeight;
		}
		else
			m_aspectRatio = (float)clientWidth / (float)clientHeight;

		CalculateProj();
	}
	void CameraComponent::SetFov(float fovY)
	{
		m_fovY = fovY;
		CalculateProj();
	}
	void CameraComponent::SetNearPlane(float nearPlane)
	{
		m_nearPlane = nearPlane;
		CalculateProj();
	}
	void CameraComponent::SetFarPlane(float farPlane)
	{
		m_farPlane = farPlane;
		CalculateProj();
	}
	void CameraComponent::SetProperties(float fovY, float nearPlane, float farPlane)
	{
		m_fovY = fovY;
		m_nearPlane = nearPlane;
		m_farPlane = farPlane;

		CalculateProj();
	}
	void CameraComponent::UpdateFrustum()
	{
		ComputePlanes();
		NormalizePlanes();
	}
	bool CameraComponent::IsVisibleInRadius(XMFLOAT3 center, float radius)
	{
		for (int i = 0; i < 6; ++i)
		{
			XMVECTOR plane = XMLoadFloat4(&m_planes[i]);

			//Check the shortest distance between the plane and the center
			float distance = XMVectorGetX(XMPlaneDotCoord(plane, XMLoadFloat3(&center)));

			if (distance < -radius)
				return false;
		}

		return true;
	}
	void CameraComponent::CalculateProj()
	{
		DirectX::XMMATRIX proj = DirectX::XMMatrixPerspectiveFovLH(m_fovY, m_aspectRatio, m_nearPlane, m_farPlane);
		DirectX::XMStoreFloat4x4(&m_proj, proj);
	}

	void CameraComponent::ComputePlanes()
	{
		XMFLOAT4X4 vp;

		XMMATRIX viewProj = XMMatrixMultiply(XMLoadFloat4x4(&m_view), XMLoadFloat4x4(&m_proj));
		XMStoreFloat4x4(&vp, viewProj);

		// Left
		m_planes[0].x = vp._14 + vp._11;
		m_planes[0].y = vp._24 + vp._21;
		m_planes[0].z = vp._34 + vp._31;
		m_planes[0].w = vp._44 + vp._41;

		// Right
		m_planes[1].x = vp._14 - vp._11;
		m_planes[1].y = vp._24 - vp._21;
		m_planes[1].z = vp._34 - vp._31;
		m_planes[1].w = vp._44 - vp._41;

		// Top
		m_planes[2].x = vp._14 - vp._12;
		m_planes[2].y = vp._24 - vp._22;
		m_planes[2].z = vp._34 - vp._32;
		m_planes[2].w = vp._44 - vp._42;

		// Bottom
		m_planes[3].x = vp._14 + vp._12;
		m_planes[3].y = vp._24 + vp._22;
		m_planes[3].z = vp._34 + vp._32;
		m_planes[3].w = vp._44 + vp._42;

		// Near
		m_planes[4].x = vp._13;
		m_planes[4].y = vp._23;
		m_planes[4].z = vp._33;
		m_planes[4].w = vp._43;

		// Far
		m_planes[5].x = vp._14 - vp._13;
		m_planes[5].y = vp._24 - vp._23;
		m_planes[5].z = vp._34 - vp._33;
		m_planes[5].w = vp._44 - vp._43;
	}

	void CameraComponent::NormalizePlanes()
	{
		for (int i = 0; i < 6; i++)
		{
			XMVECTOR plane = XMLoadFloat4(&m_planes[i]);
			plane = XMPlaneNormalize(plane);
			XMStoreFloat4(&m_planes[i], plane);
		}
	}
}

