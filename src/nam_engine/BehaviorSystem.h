#pragma once
#include "ISystem.h"

namespace nam
{
	class BehaviorSystem : public ISystem
	{
		void Update(Ecs& ecs) override;
	};
}


