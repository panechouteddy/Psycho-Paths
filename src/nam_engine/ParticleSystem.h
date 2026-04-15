#pragma once
#include "ISystem.h"
namespace nam
{
	class ParticleSystem : public ISystem
	{
	public:
		void Update(Ecs& ecs) override;
	};
}

