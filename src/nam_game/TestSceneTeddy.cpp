#include "pch.h"
#include "TestSceneTeddy.h"
#include "InteractComponent.h"

#include "BlenderSceneCustom.h"

#include "Chunk.h"
#include "Room.h"
#include "PathFinding.h"
#include "DigiCodeObject.h"

void TestSceneTeddy::Init()
{
	LoadAsset();

	mp_player = CreateGameObject<Player>();

	Mesh* p_mesh = RENDER_ITEM_MANAGER.CreateRenderItem<Mesh>();
	p_mesh->BuildBox({ 5, 1, 5 }, { 1, 1, 1, 1 });
	p_mesh->SetTexture((int)TagTexture::_Floor);

	Mesh* p_objectMesh = RENDER_ITEM_MANAGER.CreateRenderItem<Mesh>();
	p_objectMesh->BuildBox({ 1, 1, 1 }, { 1, 1, 1, 1 });
	p_objectMesh->SetTexture((int)TagTexture::_Floor);

	GameObject* object = CreateGameObject<GameObject>();

	MeshRendererComponent ObjectMeshComp;
	ObjectMeshComp.SetMeshInstance(p_objectMesh);
	object->AddComponent(ObjectMeshComp);
	object->SetTag((int)TagObject::_WallPaper);
	object->m_name = "cube";
	object->AddBoxCollider();
	object->mp_transform->SetWorldPosition({ 9,2,0 });

	Door* door = CreateGameObject<Door>();
	door->SetProperties({ 0.5f,1,0.1 }, true, true, XM_PIDIV2, 1, false);
	door->mp_transform->SetWorldPosition({ 8,0,3 });
	door->m_name = "door";
	InteractComponent ic;
	door->AddComponent(ic);

	DigiCodeObject* digicode = CreateGameObject< DigiCodeObject>();
	digicode->InitCode("0000");
	digicode->m_isActiveCode = true;
	digicode->m_digiCodeOnX = true;
	CreateGridChunks();
	mp_monster = CreateGameObject<Monster>();
	PathFindingComponent pc;
	pc.InitPathfinding(m_Grid, 1);
	mp_monster->AddComponent(pc);


}

void TestSceneTeddy::Start()
{
	AudioManager::PlayMusic("Ambient2", 1.f, true, false);

	StartGameObjects();
	SetActiveAllGameObjects(true);

	if (mp_monster->mp_pathComp)
	{
		XMFLOAT3 position = mp_monster->mp_pathComp->SetStartPosition(0, 36, 1);
		mp_monster->mp_transform->SetWorldPosition({ position.x,1,position.z });

	}
	APP->SetAmbientLightColor({ 1, 1, 1 });

	mp_player->mp_transform->SetWorldPosition({ 0, 1, 0 });

}

void TestSceneTeddy::Update()
{
	if (Input::IsKeyDown('A'))
	{
		CHRONO.SetTimeWarp(5.f);
	}
	else if (Input::IsKeyUp('A'))
	{
		CHRONO.SetTimeWarp(1.f);
	}
	if (Input::IsKeyDown('P'))
	{
		Print("x : ", mp_player->mp_transform->GetWorldPosition().x, " z : ", mp_player->mp_transform->GetWorldPosition().z);
	}
}

void TestSceneTeddy::End()
{
}

void TestSceneTeddy::LoadAsset()
{
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

	APP->LoadTexture(L"../../res/Assets/Stalker/Monster_Base_Color.dds", (int)TagTexture::_Stalker, false);
}

void TestSceneTeddy::CreateGridChunks()
{
	float roomSize = 10.f;

	Chunk* c1 = new Chunk(this);
	m_chunks.push_back(c1);

	BlenderSceneCustom::Load(L"../../res/Blender/scene_test.json", (size)TagScene::_Main, L"../../res/Blender/Textures/", c1);

	float scaleNode = 0.5f;
	float halfScaleNode = scaleNode * 0.5f;

	XMFLOAT3 chunkMin = c1->GetchunkMinIndex();
	XMFLOAT3 chunkMax = c1->GetchunkMaxIndex();

	float sizeX = chunkMax.x - chunkMin.x;
	float sizeZ = chunkMax.z - chunkMin.z;

	int nodeAmountX = (int)(sizeX / scaleNode);
	int nodeAmountZ = (int)(sizeZ / scaleNode);

	Vector <GameObject*> chunkObjects;
	for (auto& room : c1->GetAllRooms())
	{
		for (GameObject* go : room.second->GetGameObjects())
		{
			if (go->HasComponent<BoxColliderComponent>() == false)
				continue;

			chunkObjects.push_back(go);
		}
	}

	XMFLOAT3 position = { chunkMin.x + halfScaleNode, 0, chunkMin.z + halfScaleNode };
	Vector<Vector<std::pair<char, XMFLOAT3>>> grid;

	for (int x = 0; x < nodeAmountX; x++)
	{
		Vector<std::pair<char, XMFLOAT3>> col;

		position.x = chunkMin.x + halfScaleNode + x * scaleNode;

		for (int z = 0; z < nodeAmountZ; z++)
		{
			std::pair<char, XMFLOAT3> row;

			position.z = chunkMin.z + halfScaleNode + z * scaleNode;

			float margin = 1.1f;

			DirectX::BoundingBox box;
			box.Extents = { halfScaleNode * margin, 2.f, halfScaleNode * margin };
			box.Center = position;

			bool found = false;

			for (GameObject* go : chunkObjects)
			{
				if (go->IsTag((int)TagObject::_WallPaper))
				{
					int a = 0;
				}

				Mesh* p_mesh = go->GetComponent<MeshRendererComponent>().mp_mesh;

				XMFLOAT3 goPos = go->mp_transform->GetWorldPosition();
				XMVECTOR goPosV = XMLoadFloat3(&goPos);

				XMVECTOR extentsV = (XMLoadFloat3(&p_mesh->m_maxBounds) - XMLoadFloat3(&p_mesh->m_minBounds)) * 0.5f;
				XMFLOAT3 goExtents = {};
				XMStoreFloat3(&goExtents, extentsV);

				DirectX::BoundingBox goBox;
				goBox.Center = goPos;
				goBox.Extents = goExtents;

				auto result = box.Intersects(goBox);

				if (result == true)
				{
					if (!go->IsTag((int)TagObject::_Floor))
						found = false;
					else
						found = true;
				}
			}

			if (found)
			{
				std::cout << NodeType::Empty;
				row.first = NodeType::Empty;
				row.second = position;
			}
			else
			{
				std::cout << NodeType::Obstacle;
				row.first = NodeType::Obstacle;
				row.second = position;
			}
			col.push_back(row);

		}
		grid.push_back(col);
		std::cout << std::endl;
	}

	//system("pause");
	InitGridAndCheckPoint(grid);
}

void TestSceneTeddy::InitGridAndCheckPoint(Vector<Vector<std::pair<char, XMFLOAT3>>>& grid)
{
	m_Grid = new Grid;

	m_Grid->InitGrid(grid);

	if (m_Grid->GetNode(0, 1, 26)->m_isWalkable)
		m_Grid->GetNode(0, 1, 26)->m_isACheckPoint = true;
	if (m_Grid->GetNode(0, 21, 68)->m_isWalkable)
		m_Grid->GetNode(0, 21, 68)->m_isACheckPoint = true;
	if (m_Grid->GetNode(0, 36, 12)->m_isWalkable)
		m_Grid->GetNode(0, 36, 12)->m_isACheckPoint = true;
}
