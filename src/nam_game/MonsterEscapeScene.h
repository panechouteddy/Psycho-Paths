#pragma once
#include <queue>
#include <Scene.h>


class MonsterEscapeScene :  public Scene
{
	Player * mp_player = nullptr;
	Monster* mp_monster = nullptr;
	Vector<PursuitPoint> m_pursuitPoints;
public:
	void Init() override;
	void Start() override;
	void Update() override;
	void End() override;
	void CreateGrid();
	void LoadAssets();
	void CreateFloor(float x, float z);
	void CreateHWall(float x, float z);
	void CreateVWall(float x, float z);

};

