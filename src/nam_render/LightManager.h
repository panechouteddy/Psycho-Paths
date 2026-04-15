#pragma once
#include <ContainerSequential.h>
#include "Light.h"
#include "Constants.h"

namespace nam
{
	class LightManager
	{
		static Queue<u8> m_freeIDs;
		UnMap<u8, Light*> m_lights = {};
		UnMap<size, Light*> m_taggedLights = {};

	public:
		LightManager();
		~LightManager();

		Light* CreateLight();
		bool RemoveLight(u8 lightID);

		//Set the pointer to nullptr if succes
		bool RemoveLight(Light*& p_light);

		void FillBufferLight(Light* bufferTab);
		void SetRenderItemTag(Light* p_light, size tag);

		Light* GetLight(u8 lightID);
		Light* GetLight(size tag);
		u32 GetLightsCount() const;
	};
}


