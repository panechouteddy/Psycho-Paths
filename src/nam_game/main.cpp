#include "pch.h"
#include "main.h"
#include <Components.h>
#include <Systems.h>

#include "TestScene.h"
#include <PathFindingSystem.h>
#include "InteractorSystem.h"

#include "PathFindTestScene.h"

#include "TestSceneNF.h"
#include "TestSceneTeddy.h"
#include "TestSceneLilian.h"

#include "GameScene.h"
#include "MenuScene.h"
#include "GameOverScene.h"
#include "OptionsScene.h"
#include "PauseScene.h"
#include "InventoryScene.h"
#include "EndCreditsScene.h"

#include "MonsterEscapeScene.h"

#include "SaveManager.h"

using namespace nam;

inline static void LoadCommonAssets(App* p_app)
{
	MATERIAL_MANAGER.CreateMaterial((int)TagMaterial::_METAL, 0.f, 0.5f);
	MATERIAL_MANAGER.CreateMaterial((int)TagMaterial::_MONSTER, 0.f, 0.f, { 1, 1, 1 });
	MATERIAL_MANAGER.CreateMaterial((int)TagMaterial::_ITEM, 0.f, 0.25f, { 1, 1, 1 });

	APP->LoadTexture(L"wall.dds", (int)TagTexture::_Wall, true);
	APP->LoadTexture(L"carrelage.dds", (int)TagTexture::_Floor, true);
	APP->LoadTexture(L"wood.dds", (int)TagTexture::_Wood, true);

	APP->LoadTexture(L"../../res/Sprites/sprintBar.dds", (int)TagTexture::_SprintBar, false);
	APP->LoadTexture(L"../../res/Sprites/battery.dds", (int)TagTexture::_BatteryCharge, false);
	APP->LoadTexture(L"../../res/Assets/Stalker/Monster_Base_Color.dds", (int)TagTexture::_Stalker, false);

	APP->LoadTexture(L"../../res/Assets/Stalker/Monster_Base_Color.dds", (int)TagTexture::_Stalker, false);

	AudioManager::LoadMusic("Seeking", "../../res/Musics/seeking.wav", 0.4f);
	AudioManager::LoadMusic("Ambient2", "../../res/Musics/ambient2.wav", 0.25f);
	AudioManager::LoadMusic("Intense", "../../res/Musics/intense.wav", 0.75f);

	AudioManager::LoadSound("Flashlight", "../../res/Sfx/flashlight.wav", 1.5f);
	AudioManager::LoadSound("FlashReload", "../../res/Sfx/flash_reload.wav", 0.5f);

	AudioManager::LoadSound("Run", "../../res/Sfx/run.wav", 0.75f);
	AudioManager::LoadSound("Walk", "../../res/Sfx/walk.wav", 0.5f);

	AudioManager::LoadSound("Jumpscare2", "../../res/Sfx/jumpscare2.wav", 1.f);
	AudioManager::LoadSound("Laughs", "../../res/Sfx/laughs.wav", 6.f);

	AudioManager::LoadSound("DoorOpen", "../../res/Sfx/door_open.wav", 0.5f);
	AudioManager::LoadSound("DoorLocked", "../../res/Sfx/door_locked.wav", 0.5f);
	AudioManager::LoadSound("DoorOpenLocked", "../../res/Sfx/door_open_locked.wav", 0.75f);
}

inline static void LoadFontSheets(App* p_app)
{
	FontManager::LoadFnt("Sen", "../../res/Font Sheets/sen.fnt");
	p_app->LoadTexture(L"../../res/Font Sheets/sen.dds", (int)TagTexture::_FontSen, false);

	FontManager::LoadFnt("Spooky", "../../res/Font Sheets/spooky.fnt");
	p_app->LoadTexture(L"../../res/Font Sheets/spooky.dds", (int)TagTexture::_FontSpooky, false);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int cmdShow)
{
	App* app = App::Get(hInstance, 1280, 720);
	LoadFontSheets(app);
	LoadCommonAssets(app);

	SaveManager::GetPlaytimeFromFile();

	ColliderSystem* cs = ECS.AddSystem<ColliderSystem>();
	cs->AddRestriction((int)TagCollision::_Decor, (int)TagCollision::_Decor);

	NoiseSystem* ns = ECS.AddSystem<NoiseSystem>();
	ns->SetMonsterTag((int)TagObject::_Monster);

	ECS.AddSystem<PathFindingSystem>();

	SCENE_MANAGER.CreateScene<MenuScene>((int)TagScene::_MainMenu);
	SCENE_MANAGER.SetActiveScene((int)TagScene::_MainMenu, true);

	SCENE_MANAGER.CreateScene<OptionsScene>((int)TagScene::_Options);
	SCENE_MANAGER.SetActiveScene((int)TagScene::_Options, false);

	SCENE_MANAGER.CreateScene<GameScene>((int)TagScene::_Main);
	SCENE_MANAGER.SetActiveScene((int)TagScene::_Main, false);

	SCENE_MANAGER.CreateScene<PauseScene>((int)TagScene::_Pause);
	SCENE_MANAGER.SetActiveScene((int)TagScene::_Pause, false);

	SCENE_MANAGER.CreateScene<InventoryScene>((int)TagScene::_Inventory);
	SCENE_MANAGER.SetActiveScene((int)TagScene::_Inventory, false);

	SCENE_MANAGER.CreateScene<GameOverScene>((int)TagScene::_GameOver);
	SCENE_MANAGER.SetActiveScene((int)TagScene::_GameOver, false);

	SCENE_MANAGER.CreateScene<EndCreditsScene>((int)TagScene::_EndCreditsScene);
	SCENE_MANAGER.SetActiveScene((int)TagScene::_EndCreditsScene, false);

	InteractorSystem* is = ECS.AddSystem<InteractorSystem>();
	is->InitTextInteract();

	app->Run();

	return 0;
}