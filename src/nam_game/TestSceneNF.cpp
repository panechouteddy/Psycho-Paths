#include "pch.h"
#include "TestSceneNF.h"

#include <Audio/miniaudio.h>

#include "BlenderSceneCustom.h"

#include "Chunk.h"
#include "Room.h"

#include "InteractComponent.h"
#include "SaveManager.h"

#include "Collectible.h"
#include "SavePoint.h"

#include "Battery.h"

#include "EndscreenTrigger.h"

void TestSceneNF::Init()
{
	LoadAssets();

	AudioManager::LoadSound("Vine", "../../res/Sfx/vineboom.ogg", 1.f);
	AudioManager::LoadSound("Oof", "../../res/Sfx/Oof.mp3", 1.f);

	AudioManager::LoadMusic("Windfall", "../../res/Musics/Windfall.mp3");
	AudioManager::LoadMusic("SkyHigh", "../../res/Musics/Skyhigh.mp3");

	MATERIAL_MANAGER.CreateMaterial((int)TagMaterial::_METAL, 0.f, 0.5f);
	MATERIAL_MANAGER.CreateMaterial((int)TagMaterial::_MONSTER, 0.f, 0.f, { 1, 1, 1 });

	mp_player = CreateGameObject<Player>();
	mp_player->mp_transform->SetWorldPosition({ 0, 1, 0 });

	mp_monster = CreateGameObject<Monster>();
	mp_monster->mp_transform->SetWorldPosition({ 10, 1, 0 });

	{
		Battery* b = CreateGameObject<Battery>();

		MeshRendererComponent meshComp;
		Mesh* p_mesh = meshComp.CreateMeshInstance();
		p_mesh->BuildCylinder(0.1f, 20 , 0.25f, {0, 1, 0, 1 });
		p_mesh->SetTexture((int)TagTexture::_Wood);
		b->AddComponent(meshComp);

		b->mp_transform->SetWorldPosition({6, 1, 0 });
	}
	CreateTestChunks();
}

void TestSceneNF::Start()
{
	float intensity = 0.f;
	APP->SetAmbientLightColor({ intensity, intensity, intensity });

	AudioManager::PlayMusic("Ambient2", 1.f, true, true);

	//XMFLOAT3 pos = SaveManager::GetDatas().playerPos;

	//Proto
	mp_player->mp_transform->SetWorldPosition({ 3.f, 1, 0 });

	//Final
	//mp_player->mp_transform->SetWorldPosition({ 15.f, 1, 0 });

	mp_monster->mp_transform->SetWorldPosition({ 0, 1, 0 });

	StartGameObjects();
	SetActiveAllGameObjects(true);
}

void TestSceneNF::Update()
{
	SaveManager::UpdateDisplay(CHRONO.GetScaledDeltaTime());

	if (Input::IsKeyDown('P'))
	{
		SaveManager::SaveDatasInFile();
	}
}

void TestSceneNF::End()
{
	AudioManager::StopCurrentMusic();
}

void TestSceneNF::LoadAssets()
{
	APP->LoadTexture(L"wall.dds", (int)TagTexture::_Wall, true);
	APP->LoadTexture(L"carrelage.dds", (int)TagTexture::_Floor, true);
	APP->LoadTexture(L"wood.dds", (int)TagTexture::_Wood, true);

	APP->LoadTexture(L"../../res/Sprites/sprintBar.dds", (int)TagTexture::_SprintBar, false);
	APP->LoadTexture(L"../../res/Sprites/battery.dds", (int)TagTexture::_BatteryCharge, false);
}

void TestSceneNF::CreateTestChunks()
{
	Chunk* c1 = new Chunk(this);
	m_chunks[0] = c1;

	//Proto
	BlenderSceneCustom::Load(L"../../res/Blender/scene_test.json", (size)TagScene::_Main, L"../../res/Blender/Textures/", c1);

	//Final
	//BlenderSceneCustom::Load(L"../../res/Blender/FinalScene/scene_test.json", (size)TagScene::_Main, L"../../res/Blender/FinalScene/Textures/", c1);
}