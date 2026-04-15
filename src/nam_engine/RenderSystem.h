#pragma once
#include "ISystem.h"

namespace nam
{
	class RenderManager;

	class RenderSystem : public ISystem
	{
	public:
		void Update(Ecs& ecs) override;
	};
}