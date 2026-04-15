#include "pch.h"
#include "Timer.h"

namespace nam
{
	Timer::Timer(float targetTime, bool countdownMode, bool isPaused)
	{
		Init(targetTime, countdownMode, isPaused);
	}

	void Timer::Init(float targetTime, bool countdownMode, bool isPaused)
	{
		m_targetTime = targetTime;
		m_countdownMode = countdownMode;
		m_isPaused = isPaused;

		ResetProgress();
	}
	void Timer::Update(float dt)
	{
		if (m_isPaused)
			return;

		if (m_countdownMode == false)
		{
			if (m_progress >= m_targetTime)
				m_progress = m_targetTime;
			else
				m_progress += dt;
		}
		else
		{
			if (m_progress <= 0)
				m_progress = 0.f;
			else
				m_progress -= dt;
		}
	}

	void Timer::Pause()
	{
		m_isPaused = true;
	}
	void Timer::Resume()
	{
		m_isPaused = false;
	}

	const bool Timer::IsPaused() const
	{
		return m_isPaused;
	}

	void Timer::SetProgress(float value)
	{
		m_progress = std::clamp(value, 0.f, GetTargetTime());
	}

	const float Timer::GetProgress() const
	{
		return m_progress;
	}

	void Timer::ResetProgress()
	{
		(m_countdownMode) ? m_progress = m_targetTime : m_progress = 0.f;
	}

	const float Timer::GetTargetTime() const
	{
		return m_targetTime;
	}
	void Timer::SetTargetTime(float time)
	{
		ResetProgress();
		m_targetTime = time;
	}
	bool Timer::IsTargetReached() const
	{
		if (m_countdownMode == false)
			return m_progress >= m_targetTime;

		else
			return m_progress <= 0;
	}

	const float Timer::GetRatioProgress() const
	{
		if (m_countdownMode == false)
		{
			if (m_targetTime == 0)
				return -1;

			return std::clamp(m_progress / m_targetTime, 0.f, 1.f);
		}
		else
		{
			if (m_progress == 0)
				return -1;

			return std::clamp(m_targetTime / m_progress, 0.f, 1.f);
		}
	}

	std::string Timer::GetDisplay() const
	{
		std::string txt = "";

		if (m_countdownMode == false)
		{
			txt = "Time : " + std::to_string(m_progress) + " / " + std::to_string(m_targetTime) + "\n";
		}
		else
		{
			txt = "Time Left : " + std::to_string(m_progress) + "\n";
		}

		return txt;
	}
}
