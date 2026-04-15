#pragma once

namespace nam
{
	class AppChrono
	{
	private:
		LARGE_INTEGER m_frequency = {}; // ticks / seconds

		float m_timeWarpScale = 1.f;

		float m_realDeltaTime = -1;
		float m_scaledDeltaTime = -1; // Affected by timewarp

		bool m_isPaused = false;

		//Pause the scaled time and scaled dt only
		//Allows to keep real time interactions and only pauses the simulation
		bool m_isFrozen = false; 

		LARGE_INTEGER m_currentTime = {};
		LARGE_INTEGER m_prevTime = {};

		double m_realTotalTime = 0.0; // real time elapsed since the last reset
		double m_scaledTotalTime = 0.0; // time elapsed affected by timewarp since the last reset

		void UpdateTimers();
	public:
		AppChrono();

		void Reset();

		const double GetRealTotalTime() const;
		const double GetScaledTotalTime() const;

		const float GetRealDeltaTime() const;
		const float GetScaledDeltaTime() const;

		void Update();

		void SetTimeWarp(float value); // Time Multiplier

		//Pause the scaled time and real dt
		void SetPauseState(bool state);
		void TogglePause();

		//Pause the scaled time and scaled dt only
		//Allows to keep real time interactions and only pauses the simulation
		void SetFreezeState(bool state);

		//Pause the scaled time and scaled dt only
		//Allows to keep real time interactions and only pauses the simulation
		void ToggleFreezeState();

		int GetFPS();

		const float GetTimeWarpScale() const;
		const bool IsPaused() const;
		const bool IsFrozen() const;
	};
}