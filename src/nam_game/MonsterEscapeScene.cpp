#include "pch.h"
#include "MonsterEscapeScene.h"

void MonsterEscapeScene::Init()
{
	APP->SetAmbientLightColor({ 1.f,1.f,1.f });
	LoadAssets();
	CreateGrid();
	mp_player = CreateGameObject<Player>();
	mp_monster = CreateGameObject<Monster>();

	CreateFloor(5, 5);
	GameObject* object = CreateGameObject<GameObject>();

	MeshRendererComponent ObjectMeshComp;
	Mesh* p_mesh = ObjectMeshComp.CreateMeshInstance();
	p_mesh->BuildBox({ 10, 1, 10 }, { 1, 1, 1, 1 });
	object->AddComponent(ObjectMeshComp);
	object->SetTag((int)TagObject::_Floor);
	object->m_name = "cube";
	object->AddBoxCollider((int)TagCollision::_Decor);
	object->mp_transform->SetWorldPosition({ 4,0,4 });

	PhysicComponent physc;
	physc.m_isKinematic = true;
	physc.m_useGravity = false;
	object->AddComponent(physc);

	PathFindingComponent pathc;
	mp_monster->AddComponent(pathc);
}

void MonsterEscapeScene::Start()
{
	StartGameObjects();
	SetActiveAllGameObjects(true);

	mp_player->mp_transform->SetWorldPosition({ 0,4,0 });

	mp_monster->mp_transform->SetWorldPosition({ 0,0,-5 });
	mp_monster->AddPursuitPoints(m_pursuitPoints);
	mp_monster->ActivatePursuit();
}

void MonsterEscapeScene::Update()
{
}

void MonsterEscapeScene::End()
{
}

void MonsterEscapeScene::LoadAssets()
{
	MATERIAL_MANAGER.CreateMaterial((int)TagMaterial::_METAL, 0.f, 0.5f);
	MATERIAL_MANAGER.CreateMaterial((int)TagMaterial::_MONSTER, 0.f, 0.f, { 1, 1, 1 });

	APP->LoadTexture(L"wall.dds", (int)TagTexture::_Wall, true);
	APP->LoadTexture(L"carrelage.dds", (int)TagTexture::_Floor, true);
	APP->LoadTexture(L"wood.dds", (int)TagTexture::_Wood, true);

	APP->LoadTexture(L"../../res/Sprites/sprintBar.dds", (int)TagTexture::_SprintBar, false);
	APP->LoadTexture(L"../../res/Sprites/battery.dds", (int)TagTexture::_BatteryCharge, false);

	AudioManager::LoadMusic("Seeking", "../../res/Musics/seeking.wav", 0.4f);
	AudioManager::LoadMusic("Ambient2", "../../res/Musics/ambient2.wav", 0.3f);

	AudioManager::LoadSound("Flashlight", "../../res/Sfx/flashlight.wav", 1.5f);
	AudioManager::LoadSound("Jumpscare2", "../../res/Sfx/jumpscare2.wav", 1.f);
	AudioManager::LoadSound("Laughs", "../../res/Sfx/laughs.wav", 4.f);

	APP->LoadTexture(L"../../res/Font Sheets/sen.dds", (int)TagTexture::_FontSen, false);
}
void MonsterEscapeScene::CreateGrid()
{
	Mesh* WallMesh = RENDER_ITEM_MANAGER.CreateRenderItem<Mesh>();
	WallMesh->BuildBox({ 1, 1, 0.1f }, { 1,1,1,1 });

	Vector<Vector<char>> grid =
	{
		{'1',' ',' ',' ',' ',' ',' ','2'},
		{'*','*','*','*','*','*','*',' '},
		{'5',' ',' ',' ',' ','6','|',' '},
		{' ','|','*','*','*',' ','|',' '},
		{' ','|',' ',' ','|',' ','|',' '},
		{' ','|','|','8',' ','7','|',' '},
		{' ','*','*','*','*','*','*',' '},
		{'4',' ',' ',' ',' ',' ',' ','3'},
	};

//	CreateFloor(grid.size(), grid[0].size());

	for (int i = 0; i < grid.size(); i++)
	{
		for (int j = 0; j < grid[i].size(); j++)
		{
			if (grid[i][j] == '*')
				CreateVWall(i, j);
				
			else if (grid[i][j] == '|')
				CreateHWall(i, j);

			else if (grid[i][j] != ' ')
			{
				PursuitPoint pp ;
				pp.position = { (float)i,1,(float)j };
				pp.value = grid[i][j];
				m_pursuitPoints.push_back(pp);
			}

		}
	}

}

void MonsterEscapeScene::CreateFloor(float x, float z)
{
	GameObject* p_floor = CreateGameObject<GameObject>();
	p_floor->SetTag((int)TagObject::_Floor);

	MeshRendererComponent meshComp;
	Mesh* mesh = meshComp.CreateMeshInstance();
	mesh->BuildBox({x, 1, z}, { 1, 1, 1, 1 });
	mesh->SetTexture((int)TagTexture::_Floor);

	p_floor->AddComponent(meshComp);
	p_floor->AddBoxCollider((int)TagCollision::_Decor);


	p_floor->AddComponent<PhysicComponent>(PhysicComponent());
	PhysicComponent& pc = p_floor->GetComponent<PhysicComponent>();
	pc.m_useGravity = false;
	pc.m_isKinematic = true;

	XMFLOAT3 floorPos = { (int)x*0.5f ,0 ,(int)z *0.5f };
	p_floor->mp_transform->SetWorldPosition({0, 0, 0});
}

void MonsterEscapeScene::CreateHWall(float x, float z)
{
	Mesh* mesh = RENDER_ITEM_MANAGER.CreateRenderItem<Mesh>();
	mesh->BuildBox({ 1,3,0.1}, { 1,1,1,1 });
	mesh->SetTexture((int)TagTexture::_Wall);
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

	XMFLOAT3 Wallpos = { x,3,z };

	p_wall->mp_transform->SetWorldPosition(Wallpos);
}

void MonsterEscapeScene::CreateVWall(float x, float z)
{
	Mesh* mesh = RENDER_ITEM_MANAGER.CreateRenderItem<Mesh>();
	mesh->BuildBox({ 1, 3, 0.1f }, { 1,1,1,1 });
	mesh->SetTexture((int)TagTexture::_Wall);
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

	XMFLOAT3 Wallpos = { x,3,z };

	p_wall->mp_transform->SetWorldPosition(Wallpos);
	p_wall->mp_transform->SetLocalYPR(XM_PIDIV2, 0, 0);
}
