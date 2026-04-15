#pragma once
#include <Scene.h>

class NoiseObject;
class Monster;

class Door;
class Locker;

class TestScene : public Scene
{
	const int m_rows = 4;
	const int m_cols = 4;
	const float m_size = 10.f;

	Player* mp_player = nullptr;
	Monster* mp_monster = nullptr;

	NoiseObject* mp_vase = nullptr;

	Door* mp_door = nullptr;

	GameObject* mp_locker = nullptr;
public:
	void Init() override;

	void Start() override;
	void Update() override;
	void End() override;

private:
	void LoadTextures();
	void CreateFloors();
	void CreateEdges();
	void CreateWalls();

};

