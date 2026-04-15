#include "pch.h"
#include "StateMachineComponent.h"

namespace nam
{
	StateMachineComponent::StateMachineComponent()
	{
		Init(nullptr, 0);
	}

	StateMachineComponent::StateMachineComponent(GameObject* owner, int stateCount)
	{
		Init(owner, stateCount);
	}

	StateMachineComponent::~StateMachineComponent()
	{
		for (Action* pAction : m_allActions)
		{
			delete pAction;
		}
	}

	void StateMachineComponent::Init(GameObject* owner, int stateCount)
	{
		m_owner = owner;
		m_currentState = -1;
		m_allActions.resize(stateCount);
	}

	void StateMachineComponent::SetState(int state)
	{
		if (m_currentState > 0 && m_currentState < m_allActions.size())
			m_allActions[m_currentState]->OnEnd(m_owner);

		m_currentState = state;

		m_allActions[m_currentState]->OnStart(m_owner);
	}

	int StateMachineComponent::GetCurrentState() const
	{
		return m_currentState;
	}

	GameObject* StateMachineComponent::GetOwner()
	{
		return m_owner;
	}

	const std::vector<Action*>& StateMachineComponent::GetAllAction() const
	{
		return m_allActions;
	}

}
