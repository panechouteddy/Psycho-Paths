#include "pch.h"
#include "ParticleSystem.h"
#include "ParticleEmitersComponent.h"
#include "Rng.h"
#include <DataShader.h>
#include <RenderManager.h>
#include <ParticleManager.h>

using namespace DirectX;

namespace nam
{
	void ParticleSystem::Update(Ecs& ecs)
	{
		float dt = App::Get()->GetChrono().GetScaledDeltaTime();
		Vector<CBufferParticleCompute> particlesToSpawn;

		ecs.ForEach<TransformComponent, ParticleEmitersComponent>([&](uint32_t entity, TransformComponent& transform, ParticleEmitersComponent& emiters) {
			transform.UpdateWorldData();

            for (size_t i = 0; i < emiters.m_maxXYZ.size(); i++)
            {
                if (!emiters.m_isActive[i])
                    continue;

                emiters.m_spawnAdvancement[i] += emiters.m_spawnRate[i] * dt;

                while (emiters.m_spawnAdvancement[i] >= 1.0f)
                {
                    CBufferParticleCompute particle;
                    XMMATRIX worldMatrix = XMLoadFloat4x4(&transform.GetWorldMatrix());

                    XMFLOAT3 localPos;
                    localPos.x = Rng::Float(emiters.m_minXYZ[i].x, emiters.m_maxXYZ[i].x);
                    localPos.y = Rng::Float(emiters.m_minXYZ[i].y, emiters.m_maxXYZ[i].y);
                    localPos.z = Rng::Float(emiters.m_minXYZ[i].z, emiters.m_maxXYZ[i].z);

                    XMFLOAT3 localDir;
                    localDir.x = Rng::Float(emiters.m_minDir[i].x, emiters.m_maxDir[i].x);
                    localDir.y = Rng::Float(emiters.m_minDir[i].y, emiters.m_maxDir[i].y);
                    localDir.z = Rng::Float(emiters.m_minDir[i].z, emiters.m_maxDir[i].z);

                    XMVECTOR localPosVec = XMLoadFloat3(&localPos);
                    XMVECTOR worldPosVec = XMVector3Transform(localPosVec, worldMatrix);
                    XMStoreFloat3(&particle.m_position, worldPosVec);

                    XMVECTOR vel = XMLoadFloat3(&localDir);
                    vel = XMVector3Normalize(vel);
                    vel = XMVectorScale(vel, Rng::Float(emiters.m_minSpeed[i], emiters.m_maxSpeed[i]));
                    XMVECTOR worldVelVec = XMVector3TransformNormal(vel, worldMatrix);
                    XMStoreFloat3(&particle.m_velocity, worldVelVec);

                    particle.m_startColor.x = emiters.m_startColor[i].x;
                    particle.m_startColor.y = emiters.m_startColor[i].y;
                    particle.m_startColor.z = emiters.m_startColor[i].z;
                    particle.m_startColor.w = 1.0f;

                    particle.m_endColor.x = emiters.m_endColor[i].x;
                    particle.m_endColor.y = emiters.m_endColor[i].y;
                    particle.m_endColor.z = emiters.m_endColor[i].z;
                    particle.m_endColor.w = 1.0f;

                    particle.m_lifeTime = emiters.m_lifeTime[i];
                    particle.m_currentLife = 0.0f;

                    particlesToSpawn.push_back(particle);

                    emiters.m_spawnAdvancement[i] -= 1.0f;
                }
            }
		});

		for (size_t i = 0; i < particlesToSpawn.size(); i++)
		{
			CBufferParticleCompute particle = particlesToSpawn[i];
			RENDER_MANAGER->GetParticleManager()->SpawnParticle(particle);
		}
	}
}

