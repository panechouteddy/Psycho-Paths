#pragma once

namespace nam
{
    class Scene;

    struct PlaceableElement
    {
        float m_weight = 1.f;           
        int m_minCount = 1;
        int m_maxCount = 1;           
        float m_minScale = 1.f;     
        float m_maxScale = 1.f;
        float m_minRadiusCluster = 1.f; 

        std::function<GameObject*(Scene* scene, const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& normal, float scale)> SpawnFunc;
    };
}