#pragma once
class Monster;

namespace nam
{
	struct Grid;
}
class PathFindingTestScene : public Scene
{
	Player* mp_player = nullptr;
	Monster* mp_monster = nullptr;
	nam::Grid* mp_Grid = nullptr;
public:
	void Init() override;
	void Start() override;
	void Update() override;
	void End() override;

private:
	void InitMap();
	void CreateGround(XMFLOAT3 pos, Mesh* mesh);
	void CreateWall(XMFLOAT3 pos, Mesh* mesh);
};