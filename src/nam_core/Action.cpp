#include "pch.h"
#include "Action.h"

namespace nam
{
    Action::~Action()
    {
        for (auto it : m_allTransitions)
        {
            delete it;
        }
    }

    Transition* Action::CreateTransition(int state)
    {
        Transition* pTransition = new Transition(state);
        m_allTransitions.push_back(pTransition);

        return pTransition;
    }

    int Action::Update(GameObject* pOwner)
    {
        OnUpdate(pOwner);

        for (const auto& t : m_allTransitions)
        {
            if (t->Try(pOwner))
            {
                return t->GetTransitionState();
            }
        }

        return -1;
    }
}
