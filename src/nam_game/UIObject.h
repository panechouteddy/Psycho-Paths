#pragma once
#include "GameObject.h"

//A Special class that automatically adapt the position of the UI when resizing the window
class UIObject : public GameObject
{
public:
	Sprite* mp_sprite = nullptr;
	Text* mp_text = nullptr;

protected:
	bool m_forceReplacement = false;

	XMFLOAT2 m_currentWinSize = { 0, 0 };

	XMFLOAT2 m_defaultScale = { 1, 1 };
	XMFLOAT2 m_defaultPos = { 0, 0 };

public:
	void OnInit() override;
	void OnStart() override;
	void OnUpdate() override;

	void CreateSprite(XMFLOAT2 pos, XMFLOAT2 size, XMFLOAT4 color, int textureTag);
	void CreateText(XMFLOAT2 pos, std::string txt, float scale, XMFLOAT4 color, int textureTag = (int)TagTexture::_FontSen, std::string fontName = "Sen");

	void SetUiPosition(XMFLOAT2 pos) override;
	void TranslateUiPosition(XMFLOAT2 t) override;
	void SetUiScale(XMFLOAT2 scale);
};

