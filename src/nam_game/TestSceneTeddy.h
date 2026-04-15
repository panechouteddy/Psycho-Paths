#pragma once
#include <Scene.h>
class Chunk;

class TestSceneTeddy : public Scene
{
	Grid* m_Grid = nullptr;
	Player* mp_player = nullptr;
	Monster* mp_monster = nullptr;
	Vector<Chunk*> m_chunks ;
public:
	void Init() override;

	void Start() override;
	void Update() override;
	void End() override;
	void LoadAsset();
	void CreateGridChunks();
	void InitGridAndCheckPoint(Vector<Vector<std::pair<char, XMFLOAT3>>>& grid);
};
