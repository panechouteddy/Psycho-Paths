#include "pch.h"
#include "Light.h"

namespace nam
{
	void Light::SetToDirectionalLight(float intensity, DirectX::XMFLOAT3 direction, DirectX::XMFLOAT3 color)
	{
		Reset();
		m_type = LightType::_Directionnal;

		m_intensity = intensity;
		m_direction = direction;
		m_color = color;
	}

	void Light::SetToPointLight(float intensity, DirectX::XMFLOAT3 position, float range, DirectX::XMFLOAT3 color)
	{
		Reset();
		m_type = LightType::_Point;

		m_intensity = intensity;
		m_position = position;
		m_range = range;
		m_color = color;
	}

	void Light::SetToSpotLight(float intensity, DirectX::XMFLOAT3 position, float range, DirectX::XMFLOAT3 direction, float beginAngleRad, float endAngleRad, DirectX::XMFLOAT3 color)
	{
		Reset();
		m_type = LightType::_Spot;

		m_intensity = intensity;
		m_position = position;
		m_range = range;
		m_direction = direction;
		m_beginAngleRad = beginAngleRad;
		m_endAngleRad = endAngleRad;
	
		m_color = color;
	}

	void Light::SetByTransform(TransformComponent& transform)
	{
		m_position = transform.GetWorldPosition();
		m_direction = transform.GetLocalAxis(_Forward);
	}

	void Light::Reset()
	{
		m_type = LightType::_Directionnal;
		m_intensity = 0;
		m_color = { 1, 1, 1 };

		m_direction = { 0, 0, 0 };

		m_position = { 0, 0, 0 };
		float m_range = 0;

		m_beginAngleRad = 0;
		m_endAngleRad = 0;
	}
}

