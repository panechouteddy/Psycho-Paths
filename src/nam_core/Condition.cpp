#include "pch.h"
#include "Condition.h"

namespace nam
{
	Condition::Condition()
	{

	}

	bool Condition::Test(GameObject* owner)
	{
		return m_expected == m_functionCondition(owner);
	}

	void Condition::SetExpected(bool expected)
	{
		m_expected = expected;
	}

	bool Condition::GetExpected()
	{
		return m_expected;
	}

	void Condition::SetFunctionCondition(std::function<bool(GameObject*)> functionCondition)
	{
		m_functionCondition = functionCondition;
	}

	std::function<bool(GameObject*)> Condition::GetFunctionCondition()
	{
		return m_functionCondition;
	}
}
