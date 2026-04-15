#pragma once
#include <Scene.h>

class Battery;
class ClimbingObject;

class TestSceneLilian : public Scene
{
	Player* mp_player = nullptr;
	Locker* mp_locker = nullptr;
	Battery* mp_battery = nullptr;
	ClimbingObject* mp_climbingObject = nullptr;
public:
	void Init() override;
	void Start() override;
	void Update() override;
	void End() override;

	void LoadTextures();

};

