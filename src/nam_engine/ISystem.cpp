#include "pch.h"
#include "ISystem.h"

namespace nam
{
	void ISystem::Init()
	{
	}

	void ISystem::Destroy()
	{
	}

	bool ISystem::IsEnabled() const
	{
		return m_enabled;
	}

	void ISystem::SetEnabled(bool enabled)
	{
		m_enabled = enabled;
	}
}