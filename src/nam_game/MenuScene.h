#pragma once
#include <Scene.h>

class Button;
class UIObject;

class MenuScene : public nam::Scene
{
	//Background
	UIObject* mp_bg = nullptr;
	UIObject* mp_textTitle = nullptr;

	UIObject* mp_playtime = nullptr;
public:
	void Init() override;

	void Start() override;
	void Update() override;

	void End() override;

private:
	void LoadAssets();
};

