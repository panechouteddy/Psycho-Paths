#include "pch.h"
#include "PathFindTestScene.h"
#include "Monster.h"

#include "Door.h"

void PathFindingTestScene::Init()
{
	APP->LoadTexture(L"wall.dds", (int)TagTexture::_Wall, true);
	APP->LoadTexture(L"carrelage.dds", (int)TagTexture::_Floor, true);
	APP->LoadTexture(L"wood.dds", (int)TagTexture::_Wood, true);
	APP->LoadTexture(L"../../res/Assets/Harpy/harpy.dds", (int)TagTexture::_Mogwai, false);

	AudioManager::LoadSound("Vine", "../../res/Sfx/vineboom.ogg", 1.f);
	AudioManager::LoadSound("Oof", "../../res/Sfx/Oof.mp3", 1.f);

	AudioManager::LoadMusic("SkyHigh", "../../res/Musics/Skyhigh.mp3");
	AudioManager::LoadMusic("Windfall", "../../res/Musics/Windfall.mp3");
	//AudioManager::PlayMusic("Windfall", 0.2f, true, true);

	MATERIAL_MANAGER.CreateMaterial((int)TagMaterial::_METAL, 0.f, 0.5f);
	MATERIAL_MANAGER.CreateMaterial((int)TagMaterial::_MONSTER, 0.f, 0.f, { 1, 1, 1 });

	mp_player = CreateGameObject<Player>();

	InitMap();
	mp_monster = CreateGameObject<Monster>();
	mp_monster->mp_transform->SetWorldPosition({ 3,1,3 });

	PathFindingComponent pfc;
	pfc.InitPathfinding(mp_Grid, 1);
	pfc.SetStartPosition(0, 1, 3);
	mp_monster->AddComponent(pfc);

	Door* m_door = CreateGameObject<Door>();
	m_door->mp_transform->SetWorldPosition({ 1, 1, 0 });
}

void PathFindingTestScene::Start()
{
	StartGameObjects();
	SetActiveAllGameObjects(true);
}

void PathFindingTestScene::Update()
{
	if (Input::IsKeyDown('T'))
	{
		mp_monster->SetSeek(false);
	}
}

void PathFindingTestScene::End()
{
}

void PathFindingTestScene::InitMap()
{

	Mesh* floorMesh = RENDER_ITEM_MANAGER.CreateRenderItem<Mesh>();
	floorMesh->BuildBox({ 1, 1, 1 }, { 1, 1, 1, 1 });
	//floorMesh->SetTexture((int)TagTexture::_Floor);

	Mesh* WallMesh = RENDER_ITEM_MANAGER.CreateRenderItem<Mesh>();
	WallMesh->BuildBox({ 1, 1, 0.1f }, { 1,1,1,1 });
	//WallMesh->SetTexture((int)TagTexture::_Wall);

	Vector<Vector<std::pair<char, XMFLOAT3>>> valueGrid;
	Vector<Vector<char>> map =
	{
		{'*','*','*','*','*','*','*','*','*','*','*','*',},
		{'*','|',' ',' ',' ',' ',' ',' ',' ',' ','|','*',},
		{'*','*','*',' ',' ',' ',' ',' ',' ',' ',' ','*',},
		{'*',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','*',},
		{'*',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','*',},
		{'*','*','*','*','*','*',' ','*','*','*','*','*',},
		{'*','|',' ',' ',' ',' ',' ',' ',' ',' ','|','*',},
		{'*','*','*','*','*','*','*','*','*','*','*','*',},
	};

	CreateGround({ (float)map.size() , 0 ,(float)map[0].size() }, floorMesh);

	for (int i = 0; i < map.size(); i++)
	{
		Vector< std::pair<char, XMFLOAT3> > row;
		for (int j = 0; j < map[i].size(); j++)
		{
			std::pair<char, XMFLOAT3> pair;
			XMFLOAT3 pos = { (float)i,0,(float)j };

			if (map[i][j] == '*')
				CreateWall(pos, WallMesh);

			pair.first = map[i][j];
			pair.second = pos;
			row.push_back(pair);
		}
		valueGrid.push_back(row);
	}

	mp_Grid = new nam::Grid;
	mp_Grid->InitGrid(valueGrid);
}

void PathFindingTestScene::CreateGround(XMFLOAT3 pos, Mesh* mesh)
{
	GameObject* p_floor = CreateGameObject<GameObject>();
	p_floor->SetTag((int)TagObject::_Floor);

	MeshRendererComponent meshComp;
	meshComp.SetMeshInstance(mesh);

	p_floor->AddComponent(meshComp);
	p_floor->AddBoxCollider((int)TagCollision::_Decor);

	PhysicComponent pc;
	pc.m_useGravity = false;
	pc.m_isKinematic = true;

	p_floor->AddComponent(pc);

	XMFLOAT3 floorPos = { pos.x * 0.5f ,pos.y ,pos.z * 0.5f };
	p_floor->mp_transform->SetWorldPosition(floorPos);
	p_floor->mp_transform->SetWorldScale({ pos.x, 1, pos.z});
}

void PathFindingTestScene::CreateWall(XMFLOAT3 pos, Mesh* mesh)
{

	GameObject* p_wall = CreateGameObject<GameObject>();

	MeshRendererComponent meshComp;
	meshComp.SetMeshInstance(mesh);
	p_wall->AddComponent(meshComp);

	p_wall->SetTag((int)TagObject::_WallIsolate);

	BoxColliderComponent bc;
	bc.m_collisionTag = (int)TagCollision::_Decor;
	p_wall->AddComponent(bc);

	PhysicComponent pc;
	pc.m_useGravity = false;
	pc.m_isKinematic = false;

	p_wall->AddComponent(pc);

	XMFLOAT3 Wallpos = { pos };
	Wallpos.y += 1.01f ;
	Wallpos.z += 1 * 0.5f;

	p_wall->mp_transform->SetWorldPosition(Wallpos);
}