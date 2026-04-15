#pragma once
#include "ISystem.h"

namespace nam
{
	class PathFindingSystem : public ISystem
	{
		void Update(Ecs& ecs) override;
	};
}
