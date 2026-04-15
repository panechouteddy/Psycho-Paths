#pragma once
#include <vector>

namespace nam
{
	class Condition;

	class Transition
	{
	protected:
		Vector<Condition*> m_allConditions;
		int m_transitionState;

	public:
		Transition(int transitionState) : m_transitionState(transitionState) {}
		~Transition();

		Condition* AddCondition(std::function<bool(GameObject*)> functionCondition, bool expected = true);

		bool Try(GameObject* owner);
		int GetTransitionState() { return m_transitionState; }
	};
}
#include "Transition.inl"