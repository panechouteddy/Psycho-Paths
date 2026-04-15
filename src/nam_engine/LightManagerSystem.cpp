#include "pch.h"
#include "LightManagerSystem.h"

#include "LightComponent.h"
#include "TransformComponent.h"

#include "Ecs.h"

#include <RenderManager.h>
#include <DataShader.h>

namespace nam
{
	void LightManagerSystem::Update(Ecs& ecs)
	{
		if (RENDER_MANAGER->IsFirstDraw())
			return;

		ecs.ForEach<LightComponent, TransformComponent>([&](uint32_t entity, LightComponent& lightComp, TransformComponent& transform)
		{
			transform.UpdateWorldData();

			Light* p_light = lightComp.mp_light;

			if (p_light == nullptr)
				return;

			p_light->SetByTransform(transform);
		});

		LightManager& lightManager = RENDER_MANAGER->GetLightManager();

		CBufferLights lights;
		lightManager.FillBufferLight(lights.m_lights);
		lights.lightsCount = lightManager.GetLightsCount();
		lights.m_ambientColor = APP->GetAmbientLightColor();

		RENDER_MANAGER->GetConstantBuffersDatas()->mp_lightCB->CopyData(0, lights);
	}
}
