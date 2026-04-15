#pragma once
#include <DirectXMath.h>

namespace nam
{
	struct Window
	{
		std::wstring m_name = L"";
		HINSTANCE m_hInstance = nullptr;
		HWND m_hwnd = nullptr;

		//current dimensions of the window
		int m_clientWidth = -1;
		int m_clientHeight = -1;

		//dimensions of the window at the Start
		int m_startWidth = -1;
		int m_startHeight = -1;

		Window(HINSTANCE& hInst, std::wstring name);

		template<typename MsgProc>
		bool Create(int clientWidth, int clientHeight, MsgProc&& func);

		DirectX::XMFLOAT2 GetSize(bool startSize = false) const;
		DirectX::XMFLOAT2 GetCenter(bool startSize = false) const;
	};
}

#include "Window.inl"