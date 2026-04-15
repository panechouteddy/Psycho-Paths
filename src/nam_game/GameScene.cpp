#include "pch.h"
#include "GameScene.h"

#include "Chunk.h"
#include "Room.h"

#include "BlenderSceneCustom.h"

#include "SaveManager.h"
#include "EndscreenTrigger.h"

void GameScene::Init()
{
	mp_grid = new Grid;
	mp_player = CreateGameObject<Player>();

	CreateGridAndChunks();

	mp_monster = CreateGameObject<Monster>();

	PathFindingComponent pc;
	pc.InitPathfinding(mp_grid, 1);
	mp_monster->AddComponent(pc);
}

void GameScene::Start()
{
	APP->SetAmbientLightColor({ 0, 0, 0 });
	AudioManager::PlayMusic("Ambient2", 1.f, true, false);

	StartGameObjects();
	SetActiveAllGameObjects(true);

	SaveDatas& dat = SaveManager::GetDatas();
	mp_player->mp_transform->SetWorldPosition(dat.playerPos);

	if (mp_monster->mp_pathComp)
	{
		XMFLOAT3 position = mp_monster->mp_pathComp->SetStartPosition({68.f, 0.36f, -0.3f});
		mp_monster->mp_transform->SetWorldPosition({ position.x, 1, position.z });
	}

	mp_monster->AddPursuitPoints(m_pursuitPoints);
}

void GameScene::Update()
{
	SaveManager::UpdateDisplay(CHRONO.GetRealDeltaTime());
}

void GameScene::End()
{
	Input::ShowMouse();
}

Grid* GameScene::GetGrid() const
{
	return mp_grid;
}

void GameScene::CreateGridAndChunks()
{
	Chunk* c1 = new Chunk(this);
	m_chunks.push_back(c1);

	BlenderSceneCustom::Load(L"../../res/Blender/FinalScene/scene_test.json", (size)TagScene::_Main, L"../../res/Blender/FinalScene/Textures/", c1);

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
				Mesh* p_mesh = go->GetComponent<MeshRendererComponent>().mp_mesh;

				if (p_mesh == nullptr)
					continue;

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

	InitGridAndCheckpoints(grid);
}

void GameScene::InitGridAndCheckpoints(Vector<Vector<std::pair<char, XMFLOAT3>>>& grid)
{
	mp_grid->InitGrid(grid);

	for (XMFLOAT3& pos : m_pathCPpositions)
	{
		Node* n = mp_grid->GetNearestNode(pos);

		if (n->m_isWalkable)
			n->m_isACheckPoint = true;
	}
}
