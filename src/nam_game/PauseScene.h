#pragma once
#include <Scene.h>

class PauseScene : public Scene
{

public: 
	void Init() override;

	void Start() override;
	void Update() override;
	void End() override;

private:
	void LoadAssets();
};

