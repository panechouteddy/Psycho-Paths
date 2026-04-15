#include "pch.h"
#include "Transition.h"

namespace nam
{
	Transition::~Transition()
	{
		for (auto it : m_allConditions)
		{
			delete it;
		}
	}

	Condition* Transition::AddCondition(std::function<bool(GameObject*)> functionCondition, bool expected)
	{
		Condition* pCondition = new Condition();

		pCondition->SetExpected(expected);
		pCondition->SetFunctionCondition(functionCondition);

		m_allConditions.push_back(pCondition);
		return pCondition;
	}

	bool Transition::Try(GameObject* owner)
	{
		for (const auto& c : m_allConditions)
		{
			if (c->Test(owner) == false)
			{
				return false;
			}
		}
		return true;
	}
}