#pragma once
#include <Light.h>

namespace nam
{
	struct LightComponent
	{
		Light* mp_light = nullptr;

	public :
		Light* CreateLightInstance();

		//Using a existing Light
		void SetLightInstance(Light* p_mesh);

		//Delete the Light member
		void DeleteLightInstance();
	};
}
