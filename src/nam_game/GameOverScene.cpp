#include "pch.h"
#include "GameOverScene.h"

#include "Button.h"

#include "UIObject.h"

#include "SaveManager.h"

void GameOverScene::Init()
{
	LoadAssets();

	m_textsKeyframes.push_back({ 0.f, " " });
	m_textsKeyframes.push_back({ 3.f, "Tu ne peux pas perdre..." });
	m_textsKeyframes.push_back({ 6.f, "Nous avons besoin de toi." });
	m_textsKeyframes.push_back({ 9.f, "Releve toi !" });

	m_animFadingTimer.Init(2.f);

	XMFLOAT2 middle = WINDOW->GetCenter(true);

	mp_bgFading = CreateGameObject<UIObject>();
	{
		mp_bgFading->CreateSprite(middle, { middle.x * 2.f, middle.y * 2.f }, {0, 0, 0, 0}, - 1);
		mp_bgFading->mp_sprite->SetDrawLayer(0);
	}

	mp_textGO = CreateGameObject<UIObject>();
	{
		mp_textGO->CreateText(middle, " ", 2.f, { 1, 1, 1, 1 }, (int)TagTexture::_FontSen, "Sen");
		mp_textGO->mp_text->SetDrawLayer(1);
	}
}

void GameOverScene::Start()
{
	SaveManager::GetDatasFromFile();
	StartGameObjects();

	m_animTextTimer.Init(10.f);

	m_animationBegin = true;
	m_animTextTimer.ResetProgress();
	m_animFadingTimer.ResetProgress();
	mp_bgFading->mp_sprite->SetColor({ 1, 1, 1, 0.f });

	AudioManager::PlayMusic("Calm2", 1.f, true, false);
}

void GameOverScene::Update()
{
	//Input to skip the animation after seeing it the first time
	if (m_firstTimeSeen == false)
	{
		GamePad* p_0 = Input::GetGamePad(0);

		if (Input::IsKeyDown(VK_LBUTTON) || p_0->IsButtonDown(KeyButton::B))
		{
			m_animTextTimer.SetTargetTime(0.f);
		}
	}

	float dt = CHRONO.GetRealDeltaTime();

	//Animation after Texts Display
	if (m_animationBegin == false)
	{
		if (m_animFadingTimer.IsTargetReached())
		{
			SCENE_MANAGER.SetActiveScene((int)TagScene::_GameOver, false);
			SCENE_MANAGER.SetActiveScene((int)TagScene::_Main, true);
			return;
		}

		m_animFadingTimer.Update(dt);
		float ratio = m_animFadingTimer.GetProgress() / m_animFadingTimer.GetTargetTime();
		mp_bgFading->mp_sprite->SetColor({ 1, 1, 1, ratio });

		return;
	}

	//When no more Texts to display
	if (m_animTextTimer.IsTargetReached())
	{
		m_firstTimeSeen = false;
		m_animationBegin = false;
		AudioManager::StopCurrentMusic();
		AudioManager::PlaySound("Revive", 1.f);
		return;
	}

	HandleTextDisplay();
	m_animTextTimer.Update(dt);
}

void GameOverScene::End()
{
}

void GameOverScene::LoadAssets()
{
	APP->LoadTexture(L"protoBG.dds", (int)TagTexture::_MenuBackground, true);

	AudioManager::LoadMusic("Calm2", "../../res/Musics/calm2.mp3", 0.75f);
	AudioManager::LoadSound("Revive", "../../res/Sfx/revive.wav", 1.f);
}

void GameOverScene::HandleTextDisplay()
{
	Text* p_text = mp_textGO->GetComponent<TextRendererComponent>().mp_text;
	float progress = m_animTextTimer.GetProgress();

	for (auto& pair : m_textsKeyframes)
	{
		if (progress >= pair.first)
		{
			p_text->SetText(pair.second);
		}
	}

	float shakeIntensity = 2.f;

	XMFLOAT2 middle = WINDOW->GetCenter(true);
	XMFLOAT2 randomShake = { Rng::Float(-1.f, 1.f) * shakeIntensity, Rng::Float(-1.f, 1.f) * shakeIntensity };

	mp_textGO->SetUiPosition({ middle.x + randomShake.x, middle.y + randomShake.y });
}
