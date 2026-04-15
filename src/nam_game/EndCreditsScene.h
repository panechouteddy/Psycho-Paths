#pragma once
#include <Scene.h>

class UIObject;

class EndCreditsScene : public Scene
{
	Timer m_animTimer;
	Timer m_spawnTitleTimer;
	Timer m_spawnLabelTimer;
	Timer m_spawnCreditsTimer;

	UIObject* mp_title = nullptr;
	UIObject* mp_label = nullptr;
	UIObject* mp_credits = nullptr;
public:
	void Init() override;
	void Start() override;
	void Update() override;
	void End() override;

private:
	void HandleAnimation(float dt);
	void LoadAssets();
};

