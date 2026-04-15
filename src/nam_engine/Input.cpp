#include "pch.h"
#include "Input.h"
#include "App.h"

using namespace DirectX;

namespace nam
{
	u8 Input::ms_keys[INPUT_AMOUNT] = { 0 };
	 
	XMFLOAT2 Input::ms_lastMousePosition = { 0,0 };
	XMFLOAT2 Input::ms_mouseDelta{ 0,0 };
	bool Input::m_isMouseVisible = true;
	float Input::m_scrollDelta = 0.f;

	//4 Controllers
	GamePad* Input::mp_controllers[MAX_CONTROLLERS] = { new GamePad(0), new GamePad(1), new GamePad(2), new GamePad(3)};

	void Input::UnInit()
	{
		memset(ms_keys, 0, INPUT_AMOUNT);

		for (size_t i = 0; i < MAX_CONTROLLERS; i++)
		{
			delete mp_controllers[i];
		}
	}

	bool Input::IsKeyDown(int key)
	{
		return ms_keys[key] == DOWN;
	}

	bool Input::IsKey(int key)
	{
		return ms_keys[key] == DOWN || ms_keys[key] == PRESSED;
	}

	bool Input::IsKeyUp(int key)
	{
		return ms_keys[key] == UP;
	}

	const int Input::GetCurrentKey()
	{
		for (size_t i = 0; i < INPUT_AMOUNT; i++)
		{
			if (ms_keys[i] == PRESSED)
				return (int)i;
		}

		return -1;
	}

	void Input::Update()
	{
		UpdateKeyStates();

		for (size_t i = 0; i < MAX_CONTROLLERS; i++)
		{
			mp_controllers[i]->Update();
		}	
	}

	void Input::UpdateMouseDelta()
	{
		POINT mousePos;
		GetCursorPos(&mousePos);

		nam::Window& window = App::Get()->GetWindow();
		ScreenToClient(window.m_hwnd, &mousePos);

		XMFLOAT2 mousePosition;
		mousePosition.x = static_cast<float>(mousePos.x);
		mousePosition.y = static_cast<float>(mousePos.y);

		ms_mouseDelta.x = mousePosition.x - ms_lastMousePosition.x;
		ms_mouseDelta.y = mousePosition.y - ms_lastMousePosition.y;

		ms_lastMousePosition = mousePosition;
	}

	void Input::SetMousePosition(DirectX::XMFLOAT2 position, bool updateMouseDelta)
	{
		nam::Window& window = App::Get()->GetWindow();
		POINT p = { static_cast<LONG>(position.x), static_cast<LONG>(position.y) };
		ClientToScreen(window.m_hwnd, &p);

		SetCursorPos(p.x, p.y);

		if (updateMouseDelta)
			UpdateMouseDelta();
		else
			ms_lastMousePosition = position;
	}

	DirectX::XMFLOAT2 Input::GetMousePostion()
	{
		POINT mousePos;
		GetCursorPos(&mousePos);

		nam::Window& window = App::Get()->GetWindow();
		ScreenToClient(window.m_hwnd, &mousePos);

		XMFLOAT2 mousePosition;
		mousePosition.x = static_cast<float>(mousePos.x);
		mousePosition.y = static_cast<float>(mousePos.y);

		return mousePosition;
	}

	DirectX::XMFLOAT2 Input::GetMouseDelta()
	{
		return ms_mouseDelta;
	}

	void Input::HideMouse()
	{
		m_isMouseVisible = false;

		while (ShowCursor(FALSE) >= 0) {}
	}

	void Input::ShowMouse()
	{
		m_isMouseVisible = true;

		while (ShowCursor(TRUE) < 0) {}
	}

	const bool Input::IsMouseVisible()
	{
		return m_isMouseVisible;
	}

	GamePad* Input::GetGamePad(int id)
	{
		if (id < 0 || id >= MAX_CONTROLLERS)
		{
			_ASSERT(false && "GamePad -> GetGamePad() id invalid !");
			return nullptr;
		}

		return mp_controllers[id];
	}

	const float Input::GetScrollDelta()
	{
		return m_scrollDelta;
	}

	void Input::UpdateKeyStates()
	{
		for (int i = 1; i < INPUT_AMOUNT - 1; i++)
		{
			if (GetAsyncKeyState(i) < 0)
			{
				switch (ms_keys[i])
				{
				case NONE:
				case UP:
					ms_keys[i] = DOWN;
					break;
				case DOWN:
					ms_keys[i] = PRESSED;
					break;
				}
			}
			else
			{
				switch (ms_keys[i])
				{
				case PRESSED:
				case DOWN:
					ms_keys[i] = UP;
					break;
				case UP:
					ms_keys[i] = NONE;
					break;
				}
			}
		}
	}
}
