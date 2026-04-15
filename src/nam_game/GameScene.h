#pragma once
#include <Scene.h>

class Chunk;

class GameScene : public Scene
{
public:
	Vector<XMFLOAT3> m_pathCPpositions = {};
	Vector<PursuitPoint> m_pursuitPoints = {};

private:
	Grid* mp_grid = nullptr;
	Player* mp_player = nullptr;
	Monster* mp_monster = nullptr;
	Vector<Chunk*> m_chunks;

public:
	void Init() override;
	void Start() override;
	void Update() override;
	void End() override;

	Grid* GetGrid() const;

private:
	void CreateGridAndChunks();
	void InitGridAndCheckpoints(Vector<Vector<std::pair<char, XMFLOAT3>>>& grid);
};

