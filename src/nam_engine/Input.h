#pragma once
#include <DirectXMath.h>

constexpr int INPUT_AMOUNT = 256;
constexpr int MAX_CONTROLLERS = 4;

namespace nam
{
	class Input
	{
		enum
		{
			NONE,
			PRESSED,
			DOWN,
			UP
		};

	private:
		static u8 ms_keys[INPUT_AMOUNT];
		static DirectX::XMFLOAT2 ms_lastMousePosition;
		static DirectX::XMFLOAT2 ms_mouseDelta;

		static GamePad* mp_controllers[MAX_CONTROLLERS];

		static bool m_isMouseVisible;

		static float m_scrollDelta;
	public:
		static void UnInit();

		static bool IsKeyDown(int key);
		static bool IsKey(int key);
		static bool IsKeyUp(int key);

		static const int GetCurrentKey();

		static void Update();
		static void UpdateMouseDelta();

		static void SetMousePosition(DirectX::XMFLOAT2 position, bool updateMouseDelta = true);
		static DirectX::XMFLOAT2 GetMousePostion();
		static DirectX::XMFLOAT2 GetMouseDelta();
		static void HideMouse();
		static void ShowMouse();
		static const bool IsMouseVisible();

		static GamePad* GetGamePad(int id);

		static const float GetScrollDelta();
	private:
		static void UpdateKeyStates();
		friend class App;
	};
}


