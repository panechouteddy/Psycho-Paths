#pragma once

#include <DirectXMath.h>
#include "Common/MathHelper.h"
#include "LightManager.h"

namespace nam
{
    struct CBufferObj3D
    {
        DirectX::XMFLOAT4X4 m_world = MathHelper::Identity4x4();
        DirectX::XMFLOAT4X4 m_invWorld = MathHelper::Identity4x4(); // Inverse Matrix
        int m_hasTexture = 0;
        int m_isSkinned = 0;
    };

    struct CBufferViewProj3D
    {
        DirectX::XMFLOAT4X4 m_view = MathHelper::Identity4x4();
        DirectX::XMFLOAT4X4 m_proj = MathHelper::Identity4x4();
        DirectX::XMFLOAT3 m_cameraPos = {};
        float time = 0.f;

        float deltaTime = 0.f;
    };

    struct CBufferLights
    {
        Light m_lights[MAX_LIGHTS] = {};

        DirectX::XMFLOAT3 m_ambientColor = { 1, 1, 1 };
        u32 lightsCount = 0;
    };

    struct CBufferMaterial
    {
        DirectX::XMFLOAT3 m_albedo = { 1, 1, 1 };
        float m_roughness = 1.f;
        float m_metalness = 0;
    };

    struct CBUfferSkinning
    {
        DirectX::XMFLOAT4X4 m_boneTransforms[256];
    };

    struct CBufferSprite2D
    {
        DirectX::XMFLOAT4X4 m_world = MathHelper::Identity4x4();
        int m_hasTexture = 0;
    };

    struct CBufferOrtho2D
    {
        DirectX::XMFLOAT4X4 m_ortho = MathHelper::Identity4x4();
        float time = -1;
    };

    struct CBufferParticleCompute
    {
        DirectX::XMFLOAT3 m_position = { 1.f, 1.f, 1.f };
        DirectX::XMFLOAT3 m_velocity = { 0.f, 1.f, 0.f };
        DirectX::XMFLOAT4 m_startColor = { 1.f, 1.f, 1.f, 1.f };
        DirectX::XMFLOAT4 m_endColor = { 1.f, 1.f, 1.f, 1.f };
        float m_lifeTime = 5.f;
        float m_currentLife = 0.f;
    };

    struct CBufferSpawn
    {
        int spawnCount = 0;
        DirectX::XMFLOAT3 padding = { 0,0,0 };
    };
}