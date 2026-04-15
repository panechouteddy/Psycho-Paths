#include "pch.h"
#include "Window.h"

namespace nam
{
	Window::Window(HINSTANCE& hInst, std::wstring name)
	{
		m_hInstance = hInst;
		m_name = name;
	}

	DirectX::XMFLOAT2 Window::GetSize(bool startSize) const
	{
		if (startSize)
			return { (float)m_startWidth, (float)m_startHeight };
		else
			return { (float)m_clientWidth, (float)m_clientHeight };
	}

	DirectX::XMFLOAT2 Window::GetCenter(bool startSize) const
	{
		if (startSize)
			return { m_startWidth * 0.5f, m_startHeight * 0.5f };
		else
			return { m_clientWidth * 0.5f, m_clientHeight * 0.5f };
	}
}