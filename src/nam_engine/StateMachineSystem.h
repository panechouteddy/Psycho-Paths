#pragma once
#include "ISystem.h"
namespace nam
{
	class StateMachineSystem : public ISystem
	{
	public:
		void Update(Ecs& ecs) override;
	};
}

