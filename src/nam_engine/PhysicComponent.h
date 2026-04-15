#pragma once
namespace nam
{
    struct PhysicComponent
    {
        float m_mass = 1.0f;
        float m_gravityScale = 1.0f;

        DirectX::XMFLOAT3 m_velocity = { 0.f,0.f,0.f };
        float m_bounciness = 0.f;

        bool m_useGravity = true;

        //Can't be repulsed
        bool m_isKinematic = false;

        void AddImpulse(const DirectX::XMFLOAT3& impulse);
        void ResetVelocity(bool affectX, bool affectY, bool affectZ);
    };
}

