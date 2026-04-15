#include "pch.h"
#include "ParticleEmitersComponent.h"
namespace nam
{
    void ParticleEmitersComponent::AddEmiter(DirectX::XMFLOAT3 maxXYZ, DirectX::XMFLOAT3 minXYZ, DirectX::XMFLOAT3 maxDir, DirectX::XMFLOAT3 minDir, DirectX::XMFLOAT3 startColor, DirectX::XMFLOAT3 endColor, float maxSpeed, float minSpeed, float lifeTime, float spawnRate)
    {
        m_maxXYZ.push_back(maxXYZ);
        m_minXYZ.push_back(minXYZ);

        m_maxDir.push_back(maxDir);
        m_minDir.push_back(minDir);

        m_startColor.push_back(startColor);
        m_endColor.push_back(endColor);

        m_maxSpeed.push_back(maxSpeed);
        m_minSpeed.push_back(minSpeed);

        m_lifeTime.push_back(lifeTime);

        m_spawnRate.push_back(spawnRate);
        m_spawnAdvancement.push_back(0.f);

        m_isActive.push_back(true);
    }
}

