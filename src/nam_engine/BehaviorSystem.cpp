#include "pch.h"
#include "BehaviorSystem.h"
#include "BehaviorComponent.h"

namespace nam
{
	void BehaviorSystem::Update(Ecs& ecs)
	{
		ecs.ForEach<BehaviorComponent>([&](u32 idEntity, BehaviorComponent& behavior) {
			if (behavior.Update != nullptr)
				behavior.Update();
		});
	}
}

