#pragma once
#include <nam_engine/TransformComponent.h>

namespace nam
{
    enum class LightType : u32
    {
        _Directionnal,
        _Point,
        _Spot,
    };

    struct Light
    {
        LightType m_type = LightType::_Directionnal;
        DirectX::XMFLOAT3 m_color = { 1, 1, 1 };

        DirectX::XMFLOAT3 m_direction = { 0, 0, 0 };
        int m_id = -1;

        DirectX::XMFLOAT3 m_position = { 0, 0, 0 };
        float pad2 = 0;

        float m_range = 0;
        float m_beginAngleRad = 0;
        float m_endAngleRad = 0;
        float m_intensity = 0;

    public: 
        void SetToDirectionalLight(float intensity, DirectX::XMFLOAT3 direction, DirectX::XMFLOAT3 color);
        void SetToPointLight(float intensity, DirectX::XMFLOAT3 position, float range, DirectX::XMFLOAT3 color);
        void SetToSpotLight(float intensity, DirectX::XMFLOAT3 position, float range, DirectX::XMFLOAT3 direction, float beginAngleRad, float endAngleRad, DirectX::XMFLOAT3 color);
      
        //DO NOT USE IN GAMEPLAY
        void SetByTransform(TransformComponent& transform);

    private:
        void Reset();
    };
}
