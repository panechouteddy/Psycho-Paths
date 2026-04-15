#include "pch.h"
#include "AppChrono.h"

namespace nam
{
	void AppChrono::UpdateTimers()
	{
		if (!m_isPaused)
		{
			m_realTotalTime += (double)m_realDeltaTime;
			m_scaledTotalTime += (double)m_scaledDeltaTime;
		}
	}

	AppChrono::AppChrono()
	{
		QueryPerformanceFrequency(&m_frequency);
		QueryPerformanceCounter(&m_prevTime);
	}

	void AppChrono::Reset()
	{
		m_realTotalTime = 0.0;
		m_scaledTotalTime = 0.0;
	}

	const double AppChrono::GetRealTotalTime() const
	{
		return m_realTotalTime;
	}
	const double AppChrono::GetScaledTotalTime() const
	{
		return m_scaledTotalTime;
	}

	const float AppChrono::GetRealDeltaTime() const
	{
		return m_realDeltaTime;
	}

	const float AppChrono::GetScaledDeltaTime() const
	{
		return m_scaledDeltaTime;
	}

	void AppChrono::Update()
	{
		QueryPerformanceCounter(&m_currentTime);

		if (m_isPaused)
		{
			m_prevTime = m_currentTime;
			m_realDeltaTime = 0.0;
			m_scaledDeltaTime = 0.0;
			return;
		}

		m_realDeltaTime = static_cast<float>(m_currentTime.QuadPart - m_prevTime.QuadPart) / static_cast<float>(m_frequency.QuadPart);
		m_realDeltaTime = min(m_realDeltaTime, 0.05f); //To prevent from Lag Spikes and Loadings

		m_prevTime = m_currentTime;

		if (m_realDeltaTime < 0)
			m_realDeltaTime = 0;

		m_scaledDeltaTime = m_realDeltaTime * static_cast<float>(m_timeWarpScale);
	
		if (m_isFrozen)
			m_scaledDeltaTime = 0.f;

		UpdateTimers();
	}

	void AppChrono::SetTimeWarp(float value)
	{
		m_timeWarpScale = value;
	}

	void AppChrono::SetPauseState(bool state)
	{
		m_isPaused = state;
	}

	void AppChrono::TogglePause()
	{
		m_isPaused = !m_isPaused;
	}

	void AppChrono::SetFreezeState(bool state)
	{
		m_isFrozen = state;
	}

	void AppChrono::ToggleFreezeState()
	{
		m_isFrozen = !m_isFrozen;
	}

	int AppChrono::GetFPS()
	{
		if (m_realDeltaTime == 0)
			return 0;

		return static_cast<int>(1.f / m_realDeltaTime);
	}

	const float AppChrono::GetTimeWarpScale() const
	{
		return m_timeWarpScale;
	}

	const bool AppChrono::IsPaused() const
	{
		return m_isPaused;
	}

	const bool AppChrono::IsFrozen() const
	{
		return m_isFrozen;
	}

}