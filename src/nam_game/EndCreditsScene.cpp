#include "pch.h"
#include "EndCreditsScene.h"

#include "UIObject.h"

void EndCreditsScene::Init()
{
	XMFLOAT2 winCenter = WINDOW->GetCenter(true);
	LoadAssets();

	m_animTimer.Init(10.f);
	m_spawnTitleTimer.Init(3.f);
	m_spawnLabelTimer.Init(2.f);
	m_spawnCreditsTimer.Init(2.f);

	mp_title = CreateGameObject<UIObject>();
	{
		mp_title->CreateText({ winCenter.x, winCenter.y * 0.8f }, "Psycho Paths", 4.f, { 0.5f, 0, 0, 0.75f }, (int)TagTexture::_FontSpooky, "Spooky");
		mp_title->mp_text->SetDrawLayer(0);
	}

	mp_label = CreateGameObject<UIObject>();
	{
		mp_label->CreateText({ winCenter.x, winCenter.y * 1.05f }, "Fin de la Demo", 2.f, { 1, 1, 1, 0.75f });
		mp_label->mp_text->SetDrawLayer(0);
	}

	mp_credits = CreateGameObject<UIObject>();
	{
		mp_credits->CreateText({ winCenter.x, winCenter.y * 1.45f }, "Developpeurs :\n\n" "   BLuSQuirel\n" "         nf73\n" "         Ted\n\n", 1, {1, 1, 0, 0.75f});
		mp_credits->mp_text->SetDrawLayer(0);
	}
}

void EndCreditsScene::Start()
{
	m_animTimer.ResetProgress();
	m_spawnTitleTimer.ResetProgress();
	m_spawnLabelTimer.ResetProgress();
	m_spawnCreditsTimer.ResetProgress();

	StartGameObjects();
	SetActiveAllGameObjects(true);

	AudioManager::PlayMusic("EndMusic", 1, true, false);
}

void EndCreditsScene::Update()
{
	if (m_animTimer.IsTargetReached())
	{
		SCENE_MANAGER.SetActiveScene((int)TagScene::_EndCreditsScene, false);
		SCENE_MANAGER.SetActiveScene((int)TagScene::_MainMenu, true);
	}
	else
	{
		float dt = CHRONO.GetRealDeltaTime();

		m_animTimer.Update(dt);
		HandleAnimation(dt);
	}
}

void EndCreditsScene::End()
{

}

void EndCreditsScene::HandleAnimation(float dt)
{
	mp_title->mp_text->SetOpacity(m_spawnTitleTimer.GetRatioProgress());
	mp_label->mp_text->SetOpacity(m_spawnLabelTimer.GetRatioProgress());
	mp_credits->mp_text->SetOpacity(m_spawnCreditsTimer.GetRatioProgress());

	if (m_spawnTitleTimer.IsTargetReached() == false)
	{
		m_spawnTitleTimer.Update(dt);
		return;
	}

	if (m_spawnLabelTimer.IsTargetReached() == false)
	{
		m_spawnLabelTimer.Update(dt);
		return;
	}

	if (m_spawnCreditsTimer.IsTargetReached() == false)
	{
		m_spawnCreditsTimer.Update(dt);
		return;
	}
}

void EndCreditsScene::LoadAssets()
{
	AudioManager::LoadMusic("EndMusic", "../../res/Musics/endmusic.mp3", 0.75f);
}