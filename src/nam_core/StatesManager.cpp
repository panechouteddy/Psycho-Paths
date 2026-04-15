#include "pch.h"
#include "StatesManager.h"

namespace nam
{
    void StatesManager::Init(Vector<size> states)
    {
        if (states.size() > 64)
        {
            _ASSERT(false && "Too many states ! 64 max");
            return;
        }

        u64 base2 = 1;

        for (size& s : states)
        {
            m_allStates[s] = base2;

            base2 <<= 1;
        }

        // Base2 to 1, 10, 100, 1000 in binary etc
    }

    void StatesManager::SetExclusiveActiveState(size state)
    {
        u64 flag = m_allStates.at(state);

        m_currentStates = flag;
    }

    bool StatesManager::IsOnlyActiveState(size state)
    {
        u64 flag = m_allStates.at(state);

        return m_currentStates == flag;
    }

    void StatesManager::ActivateState(size state)
    {
        u64 flag = m_allStates.at(state);

        m_currentStates |= flag;
    }

    void StatesManager::DesactivateState(size state)
    {
        u64 flag = m_allStates.at(state);

        m_currentStates &= ~flag;
    }

    void StatesManager::ToggleState(size state)
    {
        u64 flag = m_allStates.at(state);

        m_currentStates ^= flag;
    }

    bool StatesManager::IsActiveState(size state)
    {
        u64 flag = m_allStates.at(state);

        return (m_currentStates & flag) != 0;
    }
}

