#pragma once
#include "GameObject.h"
#include <AllTypes.h>

namespace nam
{
	struct Sprite;
}

class Button : public GameObject
{
public:
	GameObject* mp_textGO = nullptr;
	Sprite* mp_sprite = nullptr;

private:
	bool m_forceReplacement = true;

	XMFLOAT2 m_textScale = { 1, 1 };

	bool m_hovered = true;

	std::function<void()> m_onClick = nullptr;

	int m_textureIdle = -1;
	int m_texturePressed = -1;

	XMFLOAT4 m_defaultColor = { 1, 1, 1 ,1 };
	XMFLOAT4 m_color = {1, 1, 1, 1};

	XMFLOAT2 m_startPos = { 0, 0 };
	XMFLOAT2 m_size = { 0, 0 };
	int m_currWindowWidth = 0, m_currWindowHeight = 0;

public:
	void OnInit() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnDestroy() override;

	void SetProperties(XMFLOAT2 pos, XMFLOAT2 dimensions, XMFLOAT4 color, int textureIdle, int texturePressed, std::function<void()> onClick);
	void AddText(std::string txt, XMFLOAT2 scale, XMFLOAT4 color, int drawLayerText = 0);

	void SetColor(DirectX::XMFLOAT4 color);

	void SetActive(bool state) override;

	void SetUiPosition(XMFLOAT2 pos) override;
	void TranslateUiPosition(XMFLOAT2 t) override;

	void ChangeText(std::string txt);
	void SaturateColor(float scale);
};

