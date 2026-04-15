#pragma once

namespace nam
{
	class AppChrono;

	class Timer
	{
		float m_targetTime = -1.f;
		float m_progress = 0.f;

		bool m_countdownMode = false; // time's decreasing until 0, resetted to targetTime
		bool m_isPaused = false; // pause the timer

	public:
		Timer() = default;
		Timer(float targetTime, bool countdownMode = false, bool isPaused = false);

		void Init(float targetTime, bool countdownMode = false, bool isPaused = false);
		void Update(float dt);

		void Pause();
		void Resume();

		const bool IsPaused() const;

		void SetProgress(float value);
		const float GetProgress() const;
		void ResetProgress();

		const float GetTargetTime() const;
		void SetTargetTime(float time);
		bool IsTargetReached() const;

		//Give the ratio of the progression between 0 and 1
		const float GetRatioProgress() const;

		std::string GetDisplay() const;
	};
}