#include "pch.h"
#include "PhysicComponent.h"

namespace nam
{
    void PhysicComponent::AddImpulse(const DirectX::XMFLOAT3& impulse)
    {
        m_velocity.x += impulse.x / m_mass;
        m_velocity.y += impulse.y / m_mass;
        m_velocity.z += impulse.z / m_mass;
    }

    void PhysicComponent::ResetVelocity(bool affectX, bool affectY, bool affectZ)
    {
        if(affectX)
            m_velocity.x = 0.f;

        if (affectY)
            m_velocity.y = 0.f;

        if (affectZ)
            m_velocity.z = 0.f;
    }
}
