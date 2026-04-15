#include "pch.h"
#include "GamePad.h"

namespace nam
{
	GamePad::GamePad(int id)
	{
		m_id = id;
	}

	void GamePad::Update()
	{
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));

		DWORD result = XInputGetState(m_id, &state);

		if (result != ERROR_SUCCESS)
		{
			m_isConnected = false;
			return;
		}
		else
			m_isConnected = true;

		const XINPUT_GAMEPAD& pad = state.Gamepad;

		//Buttons
		for (int i = 0; i < (int)KeyButton::COUNT; i++)
		{
			m_buttons[i].m_previous = m_buttons[i].m_current;
			m_buttons[i].m_current = (pad.wButtons & XInputMapping[i]) != 0;
		}

		//Sticks
		m_lx = pad.sThumbLX / 32767.f;
		m_ly = pad.sThumbLY / 32767.f;

		m_rx = pad.sThumbRX / 32767.f;
		m_ry = pad.sThumbRY / 32767.f;

		// Deadzone
		if (fabs(m_lx) < m_deadZone)
			m_lx = 0;
		if (fabs(m_ly) < m_deadZone)
			m_ly = 0;

		if (fabs(m_rx) < m_deadZone)
			m_rx = 0;
		if (fabs(m_ry) < m_deadZone)
			m_ry = 0;

		//Triggers
		m_lt = pad.bLeftTrigger / 255.0f;
		m_rt = pad.bRightTrigger / 255.0f;
	}

	bool GamePad::IsButton(KeyButton b)
	{
		return m_buttons[(int)b].m_current;
	}
	bool GamePad::IsButtonDown(KeyButton b)
	{
		return m_buttons[(int)b].m_current && !m_buttons[(int)b].m_previous;
	}
	bool GamePad::IsButtonUp(KeyButton b)
	{
		return !m_buttons[(int)b].m_current && m_buttons[(int)b].m_previous;
	}
	float GamePad::GetLeftStickX() const
	{
		return m_lx;
	}
	float GamePad::GetLeftStickY() const
	{
		return m_ly;
	}
	float GamePad::GetRightStickX() const
	{
		return m_rx;
	}
	float GamePad::GetRightStickY() const
	{
		return m_ry;
	}
	float GamePad::GetRightTrigger() const
	{
		return m_rt;
	}
	float GamePad::GetLeftTrigger() const
	{
		return m_lt;
	}
	const bool GamePad::IsConnected() const
	{
		return m_isConnected;
	}

	const int GamePad::GetCurrentButton() const
	{
		for (size i = 0; i < (int)KeyButton::COUNT; i++)
		{
			if (m_buttons[i].m_current == true)
				return KeyButton(i);
		}

		return -1;
	}
	std::string GamePad::GetButtonName(int button)
	{
		std::string txt = "";

		switch (button)
		{
		case A:
			txt = 'A';
			break;

		case B:
			txt = 'B';
			break;

		case X:
			txt = 'X';
			break;

		case Y:
			txt = 'Y';
			break;

		case LB:
			txt = "LB";
			break;

		case RB:
			txt = "RB";
			break;

		case Back:
			txt = "Back";
			break;

		case Start:
			txt = "Start";
			break;
		case LThumb:
			txt = "Stick G";
			break;
		case RThumb:
			txt = "Stick D";
			break;
		case DPadUp:
			txt = "Haut";
			break;
		case DPadDown:
			txt = "Bas";
			break;
		case DPadLeft:
			txt = "Gauche";
			break;

		case DPadRight:
			txt = "Droite";
			break;
		}

		return txt;
	}
}