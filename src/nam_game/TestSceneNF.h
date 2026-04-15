#pragma once
#include <Scene.h>
#include <AllTypes.h>

class NoiseObject;
class Monster;

class Door;

class Room;
class Chunk;

class Player;
class Camera;

class TestSceneNF : public nam::Scene
{
	const int m_rows = 4;
	const int m_cols = 4;
	const float m_size = 5.f;

	Player* mp_player = nullptr;
	Monster* mp_monster = nullptr;
	Camera* mp_cam = nullptr;

	NoiseObject* mp_noise = nullptr;
	Door* mp_door = nullptr;

	UnMap<size, Chunk*> m_chunks;
public:
	void Init() override;

	void Start() override;
	void Update() override;
	void End() override;

private:
	void LoadAssets();

	void CreateTestChunks();
};

