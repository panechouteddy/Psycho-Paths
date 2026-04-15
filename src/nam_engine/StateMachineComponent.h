#pragma once
namespace nam
{
	class Action;

	class StateMachineComponent
	{
	private:
		Vector<Action*> m_allActions;
		int m_currentState;

		GameObject* m_owner;
	public:
		StateMachineComponent();
		StateMachineComponent(GameObject* owner, int stateCount);
		~StateMachineComponent();
		void Init(GameObject* owner, int stateCount);

		template<typename A>
		A* CreateAction(int state);

		template<typename T, typename F>
		static Condition* CreateCondition(Transition* transition, bool expected, F&& function);

		void SetState(int state);
		int GetCurrentState() const;

		GameObject* GetOwner();

		template<typename T>
		T* GetOwner();

		const Vector<Action*>& GetAllAction() const;
	};
}

#include "StateMachineComponent.inl"
