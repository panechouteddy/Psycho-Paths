#pragma once
#include <Scene.h>

class Button;
class UIObject;

class GameOverScene : public nam::Scene
{
	//To not skip the monologue the first time you see it
	bool m_firstTimeSeen = true;

	bool m_animationBegin = false;
	Timer m_animTextTimer;
	UIObject* mp_textGO = nullptr;
	Vector<std::pair<float, std::string>> m_textsKeyframes;

	Timer m_animFadingTimer;
	UIObject* mp_bgFading = nullptr;
public:
	void Init() override;

	void Start() override;
	void Update() override;

	void End() override;

private:
	void LoadAssets();
	void HandleTextDisplay();
};

