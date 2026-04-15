#pragma once
#include "ContainerSequential.h"

namespace nam
{
	class Transition;

	class Action
	{
	private:
		Vector<Transition*> m_allTransitions;

	public:
		~Action();

		Transition* CreateTransition(int state);

		int Update(GameObject* pOwner);

	protected:
		virtual void OnStart(GameObject* pOwner) = 0;
		virtual void OnUpdate(GameObject* pOwner) = 0;
		virtual void OnEnd(GameObject* pOwner) = 0;

		friend class StateMachineComponent;
	};
}

#include "Action.inl"