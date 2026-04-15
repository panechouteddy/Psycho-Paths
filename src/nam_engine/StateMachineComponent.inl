#pragma once
#include "StateMachineComponent.h"

namespace nam
{
	template<typename T>
	inline T* StateMachineComponent::GetOwner()
	{
		return (T*)m_owner;
	}

	template<typename A>
	inline A* StateMachineComponent::CreateAction(int state)
	{
		_ASSERT(state >= 0 && state < m_allActions.size());
		_ASSERT(m_allActions[state] == nullptr);

		static_assert(std::is_base_of<Action, A>::value, "A must derive from Action");

		A* pAction = new A();

		m_allActions[state] = pAction;

		return pAction;
	}

	template<typename T, typename F>
	inline static Condition* StateMachineComponent::CreateCondition(Transition* transition, bool expected, F&& function)
	{
		static_assert(std::is_base_of<GameObject, T>::value, "T must derive from GameObject");
		using FuncType = std::function<bool(GameObject*)>;

		return transition->AddCondition(FuncType(std::forward<F>(function)), expected);
	}
}