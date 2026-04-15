
namespace nam
{
	template<typename MsgProc>
	bool Window::Create(int clientWidth, int clientHeight, MsgProc&& func)
	{
		m_startWidth = clientWidth;
		m_startHeight = clientHeight;

		m_clientWidth = clientWidth;
		m_clientHeight = clientHeight;

		WNDCLASS wc = {};
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = func; 
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = m_hInstance;
		wc.hIcon = LoadIcon(0, IDI_APPLICATION);
		wc.hCursor = LoadCursor(0, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
		wc.lpszMenuName = 0;
		wc.lpszClassName = L"MainWnd";

		long dwStyle = (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

		if (!RegisterClass(&wc))
		{
			MessageBox(0, L"RegisterClass Failed.", 0, 0);
			return false;
		}

		// Compute window rectangle dimensions based on requested client area dimensions.
		RECT rect = { 0, 0, m_clientWidth, m_clientHeight };
		AdjustWindowRect(&rect, dwStyle, false);

		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;

		m_hwnd = CreateWindow(L"MainWnd", m_name.c_str(), dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, m_hInstance, 0);

		if (!m_hwnd)
		{
			MessageBox(0, L"CreateWindow Failed.", 0, 0);
			return false;
		}

		ShowWindow(m_hwnd, SW_SHOW);
		UpdateWindow(m_hwnd);

		return true;
	}
}