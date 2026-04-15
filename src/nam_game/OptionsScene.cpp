#include "pch.h"
#include "OptionsScene.h"

#include "UIObject.h"
#include "Button.h"

#include "GameKeybinds.h"
#include "GeneralConfigs.h"

void MiniScene::SetActive(bool state)
{
	for (GameObject* p_go : m_objects)
	{
		p_go->SetActive(state);
	}
}

void OptionsScene::Init()
{
	LoadAssets();

	m_scenes[_General] = MiniScene();
	InitGeneralMiniScene(m_scenes[_General]);

	m_scenes[_ForKeyboard] = MiniScene();
	InitKeyboardMiniScene(m_scenes[_ForKeyboard]);

	m_scenes[_ForController] = MiniScene();
	InitControllerMiniScene(m_scenes[_ForController]);

	InitGlobalScene();
}

void OptionsScene::Start()
{
	if (SCENE_MANAGER.IsSceneActive((int)TagScene::_Main))
		m_cameFromGame = true;
	else
		m_cameFromGame = false;

	m_lastKeyPressed = -1;
	m_lastButtonPressed = -1;
	m_currentConfig = -1;

	StartGameObjects();
	ActivateMiniScene(_General);
}

void OptionsScene::Update()
{
	ActivateMiniScene(m_currentConfig);

	//Last Input
	int key = Input::GetCurrentKey();
	if(key != -1 && key != VK_LBUTTON)
		m_lastKeyPressed = key;

	int button = Input::GetGamePad(0)->GetCurrentButton();
	if (button != -1)
		m_lastButtonPressed = button;

	//Scrolling
	float speed = 0.f;
	speed = Input::GetScrollDelta() * CHRONO.GetRealDeltaTime() * 200.f;

	if (speed == 0.f)
		return;

	float newValue = m_scrollValue + speed;
	if (newValue < -1000.f || newValue > 25.f)
		return;

	m_scrollValue += speed;
	for (auto& [name, miniscene] : m_scenes)
	{
		for (GameObject* p_go : miniscene.m_objects)
		{
			p_go->TranslateUiPosition({ 0, speed });
		}
	}	
}

void OptionsScene::End()
{

}

void OptionsScene::ActivateMiniScene(int type)
{
	if (m_currentConfig == type)
		return;

	m_currentConfig = type;

	for (auto& [key, miniscene] : m_scenes)
	{
		miniscene.SetActive(false);
	}

	m_scenes[type].SetActive(true);
}

void OptionsScene::LoadAssets()
{
	AudioManager::LoadSound("SaveButton", "../../res/Sfx/savebutton.wav", 1.f);
}

void OptionsScene::InitKeyboardMiniScene(MiniScene& s)
{
	float alphaButton = 0.15f;
	float alphaTxt = 0.75f;
	XMFLOAT2 winSize = WINDOW->GetSize(true);
	XMFLOAT2 winCenter = WINDOW->GetCenter(true);

	XMFLOAT2 buttonSize = { 100, 50 };
	XMFLOAT2 txtScale = { 1, 1 };
	XMFLOAT4 color = { 0.7f, 0.8f, 1.f, alphaButton };

	float offsetY = 0.f;

	for (BindData& data : GameKeybinds::GetAllBindsData())
	{
		UIObject* p_text = CreateGameObject<UIObject>();
		p_text->CreateText({ winCenter.x * 0.75f, 125.f + offsetY }, data.name, 1.f, {1, 1, 1, alphaTxt});
		s.m_objects.push_back(p_text);

		Button* p_key = CreateGameObject<Button>();
		p_key->SetProperties({ winCenter.x * 1.25f, 125.f + offsetY }, buttonSize, color, -1, -1,
			[this, p_key, data]()
			{
				if (m_lastKeyPressed == -1)
					return;

				GameKeybinds::m_keybinds[data.name].first = m_lastKeyPressed;

				if (GameObject* p_textGo = p_key->mp_textGO)
					p_key->ChangeText(Utils::GetASCIIstring(m_lastKeyPressed));
			});

		p_key->AddText(Utils::GetASCIIstring(data.keyboardKey), txtScale, { 1, 1, 1, alphaTxt });
		s.m_objects.push_back(p_key);

		offsetY += 60.f;
	}
}

void OptionsScene::InitControllerMiniScene(MiniScene& s)
{
	float alphaButton = 0.15f;
	float alphaTxt = 0.75f;
	XMFLOAT2 winSize = WINDOW->GetSize(true);
	XMFLOAT2 winCenter = WINDOW->GetCenter(true);

	XMFLOAT2 buttonSize = { 100, 50 };
	XMFLOAT2 txtScale = { 1, 1 };
	XMFLOAT4 color = { 0.7f, 0.8f, 1.f, alphaButton };

	float offsetY = 0.f;

	for (BindData& data : GameKeybinds::GetAllBindsData())
	{
		if (data.name == KeyAction::_MoveLeft || data.name == KeyAction::_MoveRight || data.name == KeyAction::_MoveUp || data.name == KeyAction::_MoveDown)
			continue;

		UIObject* p_text = CreateGameObject<UIObject>();
		p_text->CreateText({ winCenter.x * 0.75f, 125.f + offsetY }, data.name, 1.f, { 1, 1, 1, alphaTxt });
		s.m_objects.push_back(p_text);

		Button* p_key = CreateGameObject<Button>();
		p_key->SetProperties({ winCenter.x * 1.25f, 125.f + offsetY }, buttonSize, color, -1, -1,
			[this, p_key, data]()
			{
				if (m_lastButtonPressed == -1)
					return;

				GameKeybinds::m_keybinds[data.name].second = m_lastButtonPressed;

				if (GameObject* p_textGo = p_key->mp_textGO)
					p_key->ChangeText(GamePad::GetButtonName(m_lastButtonPressed));
			});

		p_key->AddText(GamePad::GetButtonName(data.controllerKey), txtScale, { 1, 1, 1, alphaTxt });
		s.m_objects.push_back(p_key);

		offsetY += 60.f;
	}
}

void OptionsScene::InitGeneralMiniScene(MiniScene& s)
{
	XMFLOAT2 buttonSize = { 50, 50 };
	XMFLOAT2 txtScale = { 1, 1 };
	float offsetY = 0.f;
	float step = 75.f;

	CreateSensivitySection(s, offsetY, buttonSize, txtScale);
	offsetY += step;

	CreateGlobalVolumeSection(s, offsetY, buttonSize, txtScale);
	offsetY += step;

	CreateMusicVolumeSection(s, offsetY, buttonSize, txtScale);
	offsetY += step;

	CreateSfxVolumeSection(s, offsetY, buttonSize, txtScale);
}

void OptionsScene::CreateSensivitySection(MiniScene& s, float offsetY, XMFLOAT2 buttonSize, XMFLOAT2 txtScale)
{
	auto getText = [this]() -> std::string
		{
			return GetTextDisplay("Vitesse Camera : ", GeneralConfigs::GetDatas().m_sensivityScale, 1);
		};

	UIObject* sensivity = CreateGameObject<UIObject>();
	{
		sensivity->CreateText({ winCenter.x * 0.75f, 125.f + offsetY }, getText(), 1, {1, 1, 1, 1});
		sensivity->mp_text->SetDrawLayer(0);
		s.m_objects.push_back(sensivity);
	}

	Button* minus = CreateGameObject<Button>();
	{
		minus->SetProperties({ winCenter.x * 1.25f - 30.f, 125.f + offsetY }, buttonSize, color, -1, -1,
			[getText, this, sensivity]()
			{
				GeneralConfigs::AddSensivityScale(-0.1f);
				std::string txt = getText();
				sensivity->mp_text->SetText(txt);
			});

		minus->AddText("-", txtScale, { 1, 1, 1, alphaTxt });

		s.m_objects.push_back(minus);
	}

	Button* plus = CreateGameObject<Button>();
	{
		plus->SetProperties({ winCenter.x * 1.25f + 30.f, 125.f + offsetY }, buttonSize, color, -1, -1,
			[getText, this, sensivity]()
			{
				GeneralConfigs::AddSensivityScale(+0.1f);
				std::string txt = getText();
				sensivity->mp_text->SetText(txt);
			});

		plus->AddText("+", txtScale, { 1, 1, 1, alphaTxt });

		s.m_objects.push_back(plus);
	}
}

void OptionsScene::CreateGlobalVolumeSection(MiniScene& s, float offsetY, XMFLOAT2 buttonSize, XMFLOAT2 txtScale)
{
	auto getText = [this]() -> std::string
		{
			return GetTextDisplay("Volume Global : ", GeneralConfigs::GetDatas().m_globalVolume, 1);
		};

	UIObject* globalVol = CreateGameObject<UIObject>();
	{
		globalVol->CreateText({ winCenter.x * 0.75f, 125.f + offsetY }, getText(), 1.f, {1, 1, 1, 1});
		globalVol->mp_text->SetDrawLayer(0);
		s.m_objects.push_back(globalVol);
	}

	Button* minus = CreateGameObject<Button>();
	{
		minus->SetProperties({ winCenter.x * 1.25f - 30.f, 125.f + offsetY }, buttonSize, color, -1, -1,
			[getText, this, globalVol]()
			{
				GeneralConfigs::AddGlobalVolume(-0.1f);
				std::string txt = getText();
				globalVol->mp_text->SetText(txt);
			});

		minus->AddText("-", txtScale, { 1, 1, 1, alphaTxt });

		s.m_objects.push_back(minus);
	}

	Button* plus = CreateGameObject<Button>();
	{
		plus->SetProperties({ winCenter.x * 1.25f + 30.f, 125.f + offsetY }, buttonSize, color, -1, -1,
			[getText, this, globalVol]()
			{
				GeneralConfigs::AddGlobalVolume(+0.1f);
				std::string txt = getText();
				globalVol->mp_text->SetText(txt);
			});

		plus->AddText("+", txtScale, { 1, 1, 1, alphaTxt });

		s.m_objects.push_back(plus);
	}
}

void OptionsScene::CreateMusicVolumeSection(MiniScene& s, float offsetY, XMFLOAT2 buttonSize, XMFLOAT2 txtScale)
{
	auto getText = [this]() -> std::string
		{
			return GetTextDisplay("Volume Musique : ", GeneralConfigs::GetDatas().m_musicVolume, 1);
		};

	UIObject* musicVol = CreateGameObject<UIObject>();
	{
		musicVol->CreateText({ winCenter.x * 0.75f, 125.f + offsetY }, getText(), 1.f, {1, 1, 1, 1});
		musicVol->mp_text->SetDrawLayer(0);
		s.m_objects.push_back(musicVol);
	}

	Button* minus = CreateGameObject<Button>();
	{
		minus->SetProperties({ winCenter.x * 1.25f - 30.f, 125.f + offsetY }, buttonSize, color, -1, -1,
			[getText, this, musicVol]()
			{
				GeneralConfigs::AddMusicVolume(-0.1f);
				std::string txt = getText();
				musicVol->mp_text->SetText(txt);
			});

		minus->AddText("-", txtScale, { 1, 1, 1, alphaTxt });

		s.m_objects.push_back(minus);
	}

	Button* plus = CreateGameObject<Button>();
	{
		plus->SetProperties({ winCenter.x * 1.25f + 30.f, 125.f + offsetY }, buttonSize, color, -1, -1,
			[getText, this, musicVol]()
			{
				GeneralConfigs::AddMusicVolume(+0.1f);
				std::string txt = getText();
				musicVol->mp_text->SetText(txt);
			});

		plus->AddText("+", txtScale, { 1, 1, 1, alphaTxt });

		s.m_objects.push_back(plus);
	}
}

void OptionsScene::CreateSfxVolumeSection(MiniScene& s, float offsetY, XMFLOAT2 buttonSize, XMFLOAT2 txtScale)
{
	auto getText = [this]() -> std::string
		{
			return GetTextDisplay("Volume Bruit : ", GeneralConfigs::GetDatas().m_sfxVolume, 1);
		};

	UIObject* sfxVol = CreateGameObject<UIObject>();
	{
		sfxVol->CreateText({ winCenter.x * 0.75f, 125.f + offsetY }, getText(), 1.f, { 1, 1, 1, 1 });
		sfxVol->mp_text->SetDrawLayer(0);
		s.m_objects.push_back(sfxVol);
	}

	Button* minus = CreateGameObject<Button>();
	{
		minus->SetProperties({ winCenter.x * 1.25f - 30.f, 125.f + offsetY }, buttonSize, color, -1, -1,
			[getText, this, sfxVol]()
			{
				GeneralConfigs::AddSfxVolume(-0.1f);
				std::string txt = getText();
				sfxVol->mp_text->SetText(txt);
			});

		minus->AddText("-", txtScale, { 1, 1, 1, alphaTxt });

		s.m_objects.push_back(minus);
	}

	Button* plus = CreateGameObject<Button>();
	{
		plus->SetProperties({ winCenter.x * 1.25f + 30.f, 125.f + offsetY }, buttonSize, color, -1, -1,
			[getText, this, sfxVol]()
			{
				GeneralConfigs::AddSfxVolume(+0.1f);
				std::string txt = getText();
				sfxVol->mp_text->SetText(txt);
			});

		plus->AddText("+", txtScale, { 1, 1, 1, alphaTxt });

		s.m_objects.push_back(plus);
	}
}

std::string OptionsScene::GetTextDisplay(std::string txt, float value, int decimals)
{
	return txt + Utils::GetFloatDisplay(value, decimals);
}

void OptionsScene::InitGlobalScene()
{
	XMFLOAT2 txtScale = { 1, 1 };
	XMFLOAT4 color = { 0.7f, 0.8f, 1.f, 0.15f };

	CreateUpperGlobalObjects(txtScale, color);
	CreateBottomGlobalObjects(txtScale, color);

	UIObject* p_bg = CreateGameObject<UIObject>();
	{
		p_bg->CreateSprite(winCenter, winSize, { 0.4f, 0.45f, 0.5f, 1.f }, (int)TagTexture::_MenuBackground);
		p_bg->mp_sprite->SetDrawLayer(2);
	}
}

void OptionsScene::CreateUpperGlobalObjects(XMFLOAT2 txtScale, XMFLOAT4 color)
{
	XMFLOAT2 configButtonSize = { 200.f, 75.f };

	Button* generalConfig = CreateGameObject<Button>();
	{
		generalConfig->SetProperties({ winCenter.x - 210.f, 37.5f }, configButtonSize, color, -1, -1,
			[this]() { ActivateMiniScene(_General); });

		generalConfig->AddText("General", txtScale, { 1, 1, 1, alphaTxt });
	}

	Button* keyboardConfig = CreateGameObject<Button>();
	{
		keyboardConfig->SetProperties({ winCenter.x, 37.5f }, configButtonSize, color, -1, -1,
			[this]() { ActivateMiniScene(_ForKeyboard); });

		keyboardConfig->AddText("Clavier", txtScale, { 1, 1, 1, alphaTxt });
	}

	Button* controllerConfig = CreateGameObject<Button>();
	{
		controllerConfig->SetProperties({ winCenter.x + 210.f, 37.5f }, configButtonSize, color, -1, -1,
			[this]() { ActivateMiniScene(_ForController); });

		controllerConfig->AddText("Manette", txtScale, { 1, 1, 1, alphaTxt });
	}
}

void OptionsScene::CreateBottomGlobalObjects(XMFLOAT2 txtScale, XMFLOAT4 color)
{
	XMFLOAT2 backButtonSize = { 150.f, 75.f };
	Button* back = CreateGameObject<Button>();
	{
		back->SetProperties({ winSize.x - 75.f, winSize.y - 37.5f }, backButtonSize, color, -1, -1,
			[this]()
			{	
				if (m_cameFromGame == false)
				{
					SCENE_MANAGER.SetActiveScene((int)TagScene::_Options, false);
					SCENE_MANAGER.SetActiveScene((int)TagScene::_MainMenu, true);
				}
				else
				{
					SCENE_MANAGER.SetActiveScene((int)TagScene::_Options, false);
					SCENE_MANAGER.SetActiveScene((int)TagScene::_Pause, true);
				}
			});

		back->AddText("Retour", txtScale, { 1, 1, 1, alphaTxt });
	}

	Button* save = CreateGameObject<Button>();
	{
		save->SetProperties({ winSize.x - 250.f, winSize.y - 37.5f }, backButtonSize, color, -1, -1,
			[save]()
			{	
				GameKeybinds::SaveBindsInFile();
				GeneralConfigs::SaveGeneralInFile();
				save->mp_sprite->SetColor({ 0.5f, 0.5f, 0.5f, 0.5f });
				AudioManager::PlaySound("SaveButton");
			});

		save->AddText("Sauver", txtScale, { 1, 1, 1, alphaTxt });
	}

	UIObject* p_tuto = CreateGameObject<UIObject>();
	{
		std::string p1 = "Pour modifier une touche,\nappuyez sur une de\nvotre clavier ou manette\npuis cliquez a la souris sur\nla touche d'action a remplacer.\nSection [Clavier - Manette]\n\n";
		std::string p2 = "Sauver permet de garder\nses nouveaux parametres\napres le relancement du jeu";

		p_tuto->CreateText({ winSize.x - 160.f, 130.f }, p1 + p2, 0.75f, { 1, 1, 1, alphaTxt });
		p_tuto->mp_text->SetDrawLayer(0);
	}
}
