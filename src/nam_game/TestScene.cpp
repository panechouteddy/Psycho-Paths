#include "pch.h"
#include "TestScene.h"

#include <Audio/miniaudio.h>

#include "NoiseObject.h"
#include "Monster.h"
#include "Door.h"
#include "Locker.h"

#include <BlenderSceneParser.h>

#include "InteractComponent.h"

void TestScene::Init()
{
	LoadTextures();

	AudioManager::LoadSound("Vine", "../../res/Sfx/vineboom.ogg", 1.f);
	AudioManager::LoadSound("Oof", "../../res/Sfx/Oof.mp3", 1.f);

	AudioManager::LoadMusic("Windfall", "../../res/Musics/Windfall.mp3");
	AudioManager::LoadMusic("SkyHigh", "../../res/Musics/Skyhigh.mp3");

	//AudioManager::PlayMusic("Windfall", 0.2f, true, true);

	MATERIAL_MANAGER.CreateMaterial((int)TagMaterial::_METAL, 0.f, 0.5f);
	MATERIAL_MANAGER.CreateMaterial((int)TagMaterial::_MONSTER, 0.f, 0.f, {1, 1, 1});
}

void TestScene::Start()
{
	mp_player = CreateGameObject<Player>();
	mp_player->mp_transform->SetWorldPosition({ 0, 5, 0 });

	//BlenderSceneParser::Load(L"../../res/BlenderScenes/scene_v2.json", (size)TagScene::_Main);

	/*GameObject* p_go = CreateGameObject<GameObject>();
	{
		MeshRendererComponent meshComp;
		Mesh* p_mesh = meshComp.CreateMeshInstance();

		p_mesh->LoadObj(L"../../res/Assets/Harpy/harpy.obj");
		p_mesh->SetTexture((int)TagTexture::_Mogwai);
		p_mesh->MakeNoise(1, 1, 1);
		p_go->AddComponent(meshComp);

		XMFLOAT3 pos = { 0, 0, 2 };
		p_go->mp_transform->SetWorldPosition(pos);

		p_go->AddBoxCollider();
		p_go->AddComponent(PhysicComponent());
	}*/

	mp_vase = CreateGameObject<NoiseObject>();
	{
		MeshRendererComponent meshComp;
		Mesh* p_mesh = meshComp.CreateMeshInstance();

		p_mesh->BuildCube(0.5f, { 1.f, 0.5f, 0.5f, 1 });
		p_mesh->SetMaterialTag((int)TagMaterial::_METAL);

		mp_vase->AddComponent(meshComp);

		XMFLOAT3 pos = { -1.f, 0, -1 };
		mp_vase->mp_transform->SetWorldPosition(pos);

		mp_vase->SetTag((int)TagObject::_NoiseEmitter);

		mp_vase->AddBoxCollider((int)TagCollision::_Decor);
		mp_vase->SetNoiseComponent("Oof", 1.f, 1000.f);
	}

	/*GameObject* p_wallTest = CreateGameObject<GameObject>();
	{
		Mesh* p_mesh = RENDER_ITEM_MANAGER.CreateRenderItem<Mesh>();
		p_mesh->BuildBox({ 2.f, m_size, 0.1f }, { 1, 1, 1, 1 });
		p_mesh->SetTexture((int)TagTexture::_Wall);

		MeshRendererComponent meshComp;
		meshComp.SetMeshInstance(p_mesh);
		p_wallTest->AddComponent(meshComp);
		p_wallTest->SetTag((int)TagObject::_WallIsolate);

		BoxColliderComponent bc;
		bc.m_collisionTag = (int)TagCollision::_Decor;

		p_wallTest->AddComponent(bc);

		PhysicComponent pc;
		pc.m_useGravity = false;
		pc.m_isKinematic = false;

		p_wallTest->AddComponent(pc);
		
		XMFLOAT3 pos = { -1.f, 0, -1 };
		p_wallTest->mp_transform->SetWorldPosition(pos);
	}*/

	mp_monster = CreateGameObject<Monster>();

	mp_door = CreateGameObject<Door>();
	mp_door->mp_transform->SetWorldPosition({ 0, -0.5f, 0 });

	
	CreateFloors();
	CreateEdges();
	//CreateWalls();

	Mesh* p_objectMesh = RENDER_ITEM_MANAGER.CreateRenderItem<Mesh>();
	p_objectMesh->BuildBox({ 1, 1, 1 }, { 1, 1, 1, 1 });
	p_objectMesh->SetTexture((int)TagTexture::_Floor);

	mp_locker = CreateGameObject<GameObject>();

	MeshRendererComponent ObjectMeshComp;
	ObjectMeshComp.SetMeshInstance(p_objectMesh);
	mp_locker->AddComponent(ObjectMeshComp);
	mp_locker->SetTag((int)TagObject::_WallPaper);

	mp_locker->AddBoxCollider();

	mp_locker->mp_transform->SetWorldPosition({ 0,1,0 });
	mp_locker->AddComponent<InteractComponent>(InteractComponent());

	StartGameObjects();
	SetActiveAllGameObjects(true);
}

void TestScene::Update()
{
	if (Input::IsKeyDown('T'))
	{
		mp_vase->SetActive(true);
		mp_monster->SetSeek(false);
	}
}

void TestScene::End()
{
}

void TestScene::LoadTextures()
{
	APP->LoadTexture(L"wall.dds", (int)TagTexture::_Wall, true);
	APP->LoadTexture(L"carrelage.dds", (int)TagTexture::_Floor, true);
	APP->LoadTexture(L"wood.dds", (int)TagTexture::_Wood, true);
	APP->LoadTexture(L"../../res/Assets/Harpy/harpy.dds", (int)TagTexture::_Mogwai, false);
	APP->LoadTexture(L"../../res/Sprites/sprintBar.dds", (int)TagTexture::_SprintBar, false);
	APP->LoadTexture(L"../../res/Sprites/battery.dds", (int)TagTexture::_BatteryCharge, false);
}

void TestScene::CreateFloors()
{
	const int halfR = m_rows / 2;
	const int halfC = m_cols / 2;

	Mesh* p_mesh = RENDER_ITEM_MANAGER.CreateRenderItem<Mesh>();
	p_mesh->BuildBox({ m_size, 1, m_size }, { 1, 1, 1, 1 });
	p_mesh->SetTexture((int)TagTexture::_Floor);

	for (int row = -halfR; row < halfR + 1; row++)
	{
		for (int col = -halfC; col < halfC + 1; col++)
		{
			for (int height = -1; height < 2; height += 2)
			{
				GameObject* p_floor = CreateGameObject<GameObject>();
				{
					MeshRendererComponent meshComp;
					meshComp.SetMeshInstance(p_mesh);

					p_floor->AddComponent(meshComp);

					p_floor->AddBoxCollider((int)TagCollision::_Decor);

					PhysicComponent pc;
					pc.m_useGravity = false;
					pc.m_isKinematic = true;

					p_floor->AddComponent(pc);

					XMFLOAT3 pos = { row * m_size, height * 2.f, col * m_size };
					p_floor->mp_transform->SetWorldPosition(pos);
				}

				p_floor->SetTag((int)TagObject::_Floor);
			}
		}
	}
}

void TestScene::CreateEdges()
{
	const int halfR = m_rows / 2;
	const int halfC = m_cols / 2;

	Mesh* p_mesh = RENDER_ITEM_MANAGER.CreateRenderItem<Mesh>();
	p_mesh->BuildBox({ m_size, m_size, 0.1f }, { 1, 1, 1, 1 });
	p_mesh->SetTexture((int)TagTexture::_Wall);

	// Front / Behind
	for (int row = -halfR; row <= halfR; row++)
	{
		for (int side = -1; side <= 1; side += 2)
		{
			GameObject* p_wall = CreateGameObject<GameObject>();

			MeshRendererComponent meshComp;
			meshComp.SetMeshInstance(p_mesh);
			p_wall->AddComponent(meshComp);
			p_wall->SetTag((int)TagObject::_WallPaper);

			BoxColliderComponent bc;
			bc.m_collisionTag = (int)TagCollision::_Decor;

			p_wall->AddComponent(bc);

			PhysicComponent pc;
			pc.m_useGravity = false;
			pc.m_isKinematic = true;

			p_wall->AddComponent(pc);

			float z = (halfC * m_size + m_size * 0.5f) * side;

			XMFLOAT3 pos = { row * m_size, -1, z };
			p_wall->mp_transform->SetWorldPosition(pos);
		}
	}

	// Left Right
	for (int col = -halfC; col <= halfC; col++)
	{
		for (int side = -1; side <= 1; side += 2)
		{
			GameObject* p_wall = CreateGameObject<GameObject>();

			MeshRendererComponent meshComp;
			meshComp.SetMeshInstance(p_mesh);
			p_wall->AddComponent(meshComp);
			p_wall->SetTag((int)TagObject::_WallPaper);

			BoxColliderComponent bc;
			bc.m_collisionTag = (int)TagCollision::_Decor;

			p_wall->AddComponent(bc);

			PhysicComponent pc;
			pc.m_useGravity = false;
			pc.m_isKinematic = true;

			p_wall->AddComponent(pc);

			float x = (halfR * m_size + m_size * 0.5f) * side;

			XMFLOAT3 pos = { x, 0, col * m_size };

			p_wall->mp_transform->SetWorldYPR(-XM_PIDIV2, 0, 0);
			p_wall->mp_transform->SetWorldPosition(pos);
		}
	}
}

void TestScene::CreateWalls()
{
	const int halfR = m_rows / 2;
	const int halfC = m_cols / 2;

	float density = 0.5f; //PROBA

	Mesh* p_mesh = RENDER_ITEM_MANAGER.CreateRenderItem<Mesh>();
	p_mesh->BuildBox({ m_size, m_size, 0.1f }, { 1,1,1,1 });
	p_mesh->SetTexture((int)TagTexture::_Wall);

	for (int row = -halfR; row <= halfR; row++)
	{
		for (int col = -halfC; col <= halfC; col++)
		{
			float x = row * m_size;
			float z = col * m_size;

			if (Rng::Float(0.f, 1.f) <= density)
			{
				GameObject* p_wall = CreateGameObject<GameObject>();

				MeshRendererComponent meshComp;
				meshComp.SetMeshInstance(p_mesh);
				p_wall->AddComponent(meshComp);

				p_wall->SetTag((int)TagObject::_WallIsolate);

				BoxColliderComponent bc;
				bc.m_collisionTag = (int)TagCollision::_Decor;
				p_wall->AddComponent(bc);

				PhysicComponent pc;
				pc.m_useGravity = false;
				pc.m_isKinematic = false;

				p_wall->AddComponent(pc);

				XMFLOAT3 pos = { x + m_size * 0.5f, 0, z };

				p_wall->mp_transform->SetWorldYPR(XM_PIDIV2, 0, 0);

				p_wall->mp_transform->SetWorldPosition(pos);
			}

			if (Rng::Float(0.f, 1.f) <= density)
			{
				GameObject* p_wall = CreateGameObject<GameObject>();

				MeshRendererComponent meshComp;
				meshComp.SetMeshInstance(p_mesh);
				p_wall->AddComponent(meshComp);

				p_wall->SetTag((int)TagObject::_WallIsolate);

				BoxColliderComponent bc;
				bc.m_collisionTag = (int)TagCollision::_Decor;
				p_wall->AddComponent(bc);

				PhysicComponent pc;
				pc.m_useGravity = false;
				pc.m_isKinematic = false;

				p_wall->AddComponent(pc);

				XMFLOAT3 pos = { x, 0, z + m_size * 0.5f };

				p_wall->mp_transform->SetWorldPosition(pos);
			}
		}
	}
}
