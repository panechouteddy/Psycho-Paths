#include "pch.h"
#include "LightComponent.h"

#include <LightManager.h>
#include <RenderManager.h>

namespace nam
{
	Light* LightComponent::CreateLightInstance()
	{
		mp_light = RENDER_MANAGER->GetLightManager().CreateLight();

		return mp_light;
	}
	void LightComponent::SetLightInstance(Light* p_light)
	{
		if (p_light == nullptr)
		{
			_ASSERT(false && "You setted a nullptr Light");
			return;
		}

		if (p_light->m_id == -1)
		{
			_ASSERT(false && "YOU CAN'T CREATE RAW LIGHT INSTANCES, YOU NEED TO CREATE A LIGHT\nWITH THE LightManager class : Use app->GetLightManager().CreateLight()");
			return;
		}

		mp_light = p_light;
	}
	void LightComponent::DeleteLightInstance()
	{
		RENDER_MANAGER->GetLightManager().RemoveLight(mp_light);
	}
}
