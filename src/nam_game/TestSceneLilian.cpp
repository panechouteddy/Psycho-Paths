#include "pch.h"
#include "TestSceneLilian.h"
#include "InteractComponent.h"
#include "Locker.h"
#include "Battery.h"
#include "ClimbingObject.h"

void TestSceneLilian::Init()
{
	APP->SetAmbientLightColor({ 1, 1, 1 });

	LoadTextures();
	mp_player = CreateGameObject<Player>();
	mp_player->mp_transform->SetWorldPosition({ 0, 5, 0 });
	mp_player->SetTag((int)TagObject::_Player);

	Mesh* p_mesh = RENDER_ITEM_MANAGER.CreateRenderItem<Mesh>();
	p_mesh->BuildBox({ 5, 1, 5 }, { 1, 1, 1, 1 });
	p_mesh->SetTexture((int)TagTexture::_Floor);

	Mesh* p_objectMesh = RENDER_ITEM_MANAGER.CreateRenderItem<Mesh>();
	p_objectMesh->BuildBox({ 0.5f, 1, 0.5f }, { 1, 1, 1, 1 });
	p_objectMesh->SetTexture((int)TagTexture::_Floor);

	//Locker
	mp_locker = CreateGameObject<Locker>();
	{
		MeshRendererComponent ObjectMeshComp;
		ObjectMeshComp.SetMeshInstance(p_objectMesh);
		mp_locker->AddComponent(ObjectMeshComp);
		mp_locker->mp_transform->SetWorldPosition({1, 1, 10});
	}
	
	//Battery
	mp_battery = CreateGameObject<Battery>();
	{
		MeshRendererComponent ObjectMeshComp;
		ObjectMeshComp.SetMeshInstance(p_objectMesh);
		mp_battery->AddComponent(ObjectMeshComp);
		mp_battery->mp_transform->SetWorldPosition({10, 1.f, -10});
	}

	mp_climbingObject = CreateGameObject<ClimbingObject>();
	mp_climbingObject->mp_transform->SetWorldPosition({ -3.f, 1.1f, 3.f });

	GameObject* pFloor = CreateGameObject<GameObject>();
	pFloor->mp_transform->SetWorldScale({ 5,1,5 });
	pFloor->mp_transform->SetWorldPosition({ 0,0,0 });

	MeshRendererComponent FloorMeshComp;
	FloorMeshComp.SetMeshInstance(p_mesh);
	pFloor->AddComponent(FloorMeshComp);
	pFloor->SetTag((int)TagObject::_Floor);

	pFloor->AddBoxCollider((int)TagCollision::_Decor);

	PhysicComponent floorPC;
	floorPC.m_isKinematic = true;
	floorPC.m_useGravity = false;

	pFloor->AddComponent(floorPC);

	StartGameObjects();
	SetActiveAllGameObjects(true);
}

void TestSceneLilian::Start()
{
}

void TestSceneLilian::Update()
{
}

void TestSceneLilian::End()
{
}

void TestSceneLilian::LoadTextures()
{
	APP->LoadTexture(L"wall.dds", (int)TagTexture::_Wall, true);
	APP->LoadTexture(L"carrelage.dds", (int)TagTexture::_Floor, true);
	APP->LoadTexture(L"wood.dds", (int)TagTexture::_Wood, true);
	APP->LoadTexture(L"../../res/Font Sheets/sen.dds", (int)TagTexture::_FontSen, false);
	APP->LoadTexture(L"../../res/Sprites/sprintBar.dds", (int)TagTexture::_SprintBar, false);
	APP->LoadTexture(L"../../res/Sprites/battery.dds", (int)TagTexture::_BatteryCharge, false);
}

