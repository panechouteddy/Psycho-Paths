#pragma once

namespace nam
{
	struct ButtonState
	{
		bool m_current = false;
		bool m_previous = false;
	};

	enum KeyButton
	{
		A,
		B,
		X,
		Y,
		LB,
		RB,
		Back,
		Start,
		LThumb,
		RThumb,
		DPadUp,
		DPadDown,
		DPadLeft,
		DPadRight,

		COUNT
	};

	static const int XInputMapping[(int)KeyButton::COUNT] =
	{
		XINPUT_GAMEPAD_A,
		XINPUT_GAMEPAD_B,
		XINPUT_GAMEPAD_X,
		XINPUT_GAMEPAD_Y,
		XINPUT_GAMEPAD_LEFT_SHOULDER,
		XINPUT_GAMEPAD_RIGHT_SHOULDER,
		XINPUT_GAMEPAD_BACK,
		XINPUT_GAMEPAD_START,
		XINPUT_GAMEPAD_LEFT_THUMB,
		XINPUT_GAMEPAD_RIGHT_THUMB,
		XINPUT_GAMEPAD_DPAD_UP,
		XINPUT_GAMEPAD_DPAD_DOWN,
		XINPUT_GAMEPAD_DPAD_LEFT,
		XINPUT_GAMEPAD_DPAD_RIGHT
	};

    class GamePad
    {
		ButtonState m_buttons[(int)KeyButton::COUNT];

        //Left Stick
		float m_lx = 0.f;
        float m_ly = 0.f;

		//Right Stick
		float m_rx = 0.f;
		float m_ry = 0.f;

		//Triggers
        float m_lt = 0.f;
        float m_rt = 0.f;

		int m_id = 0;
		float m_deadZone = 0.5f;
		bool m_isConnected = false;

    public:
        bool IsButton(KeyButton b);
        bool IsButtonDown(KeyButton b);
        bool IsButtonUp(KeyButton b);

		float GetLeftStickX() const;
		float GetLeftStickY() const;

		float GetRightStickX() const;
		float GetRightStickY() const;

		float GetRightTrigger() const;
		float GetLeftTrigger() const;
		
		const bool IsConnected() const;

		const int GetCurrentButton() const;

		static std::string GetButtonName(int button);

	private:
		GamePad(int id);
		void Update();

		friend class Input;
    };
}
