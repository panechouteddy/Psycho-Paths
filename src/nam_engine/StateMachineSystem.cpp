#include "pch.h"
#include "StateMachineSystem.h"

namespace nam
{
	void StateMachineSystem::Update(Ecs& ecs)
	{
		ecs.ForEach<StateMachineComponent*>([&](uint32_t entity, StateMachineComponent*& stateMachine) {
			int state = stateMachine->GetCurrentState();
			if (state == -1)
				return;

			int transitionState = stateMachine->GetAllAction()[state]->Update(stateMachine->GetOwner());

			if (transitionState == -1)
				return;

			stateMachine->SetState(transitionState);
		});
		
	}
}
