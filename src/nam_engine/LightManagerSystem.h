#pragma once
#include "ISystem.h"

namespace nam
{
	class LightManagerSystem : public ISystem
	{
	public:
		void Update(Ecs& ecs) override;
	};
}

