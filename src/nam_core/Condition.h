#pragma once

namespace nam
{
    class GameObject;
    class Transition;

    class Condition
    {
    private:
        bool m_expected;
        std::function<bool(GameObject*)> m_functionCondition;
    public:
        Condition();
        //virtual bool OnTest(T* owner) = 0;

        friend Transition;
    private:
        bool Test(GameObject* owner);

        void SetExpected(bool expected);
        bool GetExpected();

        void SetFunctionCondition(std::function<bool(GameObject*)> functionCondition);
        std::function<bool(GameObject*)> GetFunctionCondition();
    };
}
#include "Condition.inl"
