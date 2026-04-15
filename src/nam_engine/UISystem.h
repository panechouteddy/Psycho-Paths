#pragma once
#include "ISystem.h"

namespace nam
{
	class UISystem : public ISystem
	{
	public:
		void Update(Ecs& ecs) override;
	};
}

