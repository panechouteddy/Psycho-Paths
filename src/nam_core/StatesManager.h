#pragma once
#include "ContainerAssociative.h"

namespace nam
{
	class StatesManager
	{
		//first = enum pos, second = flag value
		UnMap<size, u64> m_allStates;

		//Mask
		u64 m_currentStates = 0;

	public:
		//Create a enum of size_t with all your States
		//Init all the states you'll use in a tab {IDLE, MOVING, JUMP, ...}
		void Init(Vector<size> states);

		//Set current State to this state and remove all others current states
		//You can use it if you want to use it has a Unique State Manager too
		void SetExclusiveActiveState(size state);

		//Return true if the state is the only one to be Active
		bool IsOnlyActiveState(size state);

		//Add a new state to the currents one
		void ActivateState(size state);

		//Remove a state from the currents one
		void DesactivateState(size state);

		//Toggle a state
		void ToggleState(size state);

		bool IsActiveState(size state);
	};
}

