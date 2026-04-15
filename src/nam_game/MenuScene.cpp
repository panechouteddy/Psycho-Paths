#include "pch.h"
#include "MenuScene.h"

#include "Button.h"

#include "UIObject.h"

#include "SaveManager.h"
#include "GameKeybinds.h"
#include "GeneralConfigs.h"

void MenuScene::Init()
{
	GameKeybinds::InitFromFile();
	GeneralConfigs::InitFromFile();

	LoadAssets();

	XMFLOAT2 buttonSize = { 200, 75.f };
	XMFLOAT2 middle = WINDOW->GetCenter(true);
	XMFLOAT2 txtScale = { 1.25f, 1.25f };

	float alphaButton = 0.15f;
	float alphaTxt = 0.75f;

	XMFLOAT4 color = { 0.7f, 0.8f, 1.f, alphaButton };
	Button* play = CreateGameObject<Button>();
	{
		play->SetProperties({ middle.x, middle.y * 0.9f }, buttonSize, color, -1, -1,
			[]() {
					SaveManager::GetNewDatas();
					SCENE_MANAGER.SetActiveScene((int)TagScene::_MainMenu, false);
					SCENE_MANAGER.SetActiveScene((int)TagScene::_Main, true); });

		play->AddText("Commencer", txtScale, { 1, 1, 1, alphaTxt });
	}

	Button* load = CreateGameObject<Button>();
	{
		load->SetProperties({ middle.x, middle.y * 1.2f }, buttonSize, color, -1, -1,
			[]()
			{ 
				SaveManager::GetDatasFromFile();
				SCENE_MANAGER.SetActiveScene((int)TagScene::_MainMenu, false);
				SCENE_MANAGER.SetActiveScene((int)TagScene::_Main, true); });

		load->AddText("Continuer", txtScale, { 1, 1, 1, alphaTxt });
	}

	Button* settings = CreateGameObject<Button>();
	{
		settings->SetProperties({ middle.x, middle.y * 1.5f }, buttonSize, color, -1, -1,
			[](){	SCENE_MANAGER.SetActiveScene((int)TagScene::_MainMenu, false);
					SCENE_MANAGER.SetActiveScene((int)TagScene::_Options, true); });

		settings->AddText("Options", txtScale, { 1, 1, 1, alphaTxt });
	}

	Button* exit = CreateGameObject<Button>();
	{
		exit->SetProperties({ middle.x, middle.y * 1.8f }, buttonSize, color, -1, -1,
			[]() { APP->m_isRunning = false; });

		exit->AddText("Quitter", txtScale, { 1, 1, 1, alphaTxt });
	}

	mp_bg = CreateGameObject<UIObject>();
	{
		mp_bg->CreateSprite(middle, { middle.x * 2.f, middle.y * 2.f }, { 1, 1, 1 ,1 }, (int)TagTexture::_MenuBackground);
		mp_bg->mp_sprite->SetDrawLayer(2);
	}

	mp_textTitle = CreateGameObject<UIObject>();
	{
		mp_textTitle->CreateText({ middle.x, middle.y * 0.4f }, "Psycho Paths", 4.f, { 0.5f, 0, 0, 0.75f }, (int)TagTexture::_FontSpooky, "Spooky");
		mp_textTitle->mp_text->SetDrawLayer(0);
	}

	mp_playtime = CreateGameObject<UIObject>();
	{
		mp_playtime->CreateText({ middle.x, middle.y * 1.95f }, " ", 1.f, { 1, 1, 1, alphaTxt });
		mp_playtime->mp_text->SetDrawLayer(0);
	}
}

void MenuScene::Start()
{
	SaveManager::SavePlaytimeInFile();
	SaveManager::GetPlaytimeFromFile();

	StartGameObjects();
	SetActiveAllGameObjects(true);

	AudioManager::PlayMusic("MainMenu", 1.f, true, false);

	u64 playtime = (int)SaveManager::m_playTime;
	u64 hours = 0, min = 0, sec = 0;

	sec = playtime;

	min = sec / 60;
	sec -= min * 60;

	hours = min / 60;
	min -= hours * 60;
	sec -= min / 60;

	std::string display = "Temps de jeu : " + std::to_string(hours) + std::string("h ")
										+ std::to_string(min) + std::string("m ")
										+ std::to_string(sec) + std::string("s");

	mp_playtime->mp_text->SetText(display);
}

void MenuScene::Update()
{
	float total = (float)CHRONO.GetRealTotalTime();

	float intensity = 0.25f;
	float factor = 1 + cos(total) * intensity;
	mp_bg->GetComponent<SpriteRendererComponent>().mp_sprite->SetColor({ 0.8f * factor, 0.9f * factor, 1.f * factor, 0.5f });
}

void MenuScene::End()
{
	AudioManager::StopCurrentMusic();
}

void MenuScene::LoadAssets()
{
	APP->LoadTexture(L"protoBG.dds", (int)TagTexture::_MenuBackground, true);

	AudioManager::LoadMusic("MainMenu", "../../res/Musics/mainmenu.wav", 0.3f);

	AudioManager::LoadSound("ButtonHover", "../../res/Sfx/hover.wav", 0.25f);
	AudioManager::LoadSound("ButtonRelease", "../../res/Sfx/release.wav", 0.25f);
}
