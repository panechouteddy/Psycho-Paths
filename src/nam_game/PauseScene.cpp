#include "pch.h"
#include "PauseScene.h"

#include "Button.h"
#include "UIObject.h"

void PauseScene::Init()
{
	XMFLOAT2 winSize = WINDOW->GetSize(true);
	XMFLOAT2 winCenter = WINDOW->GetCenter(true);

	XMFLOAT2 buttonSize = { 125.f, 75.f };
	XMFLOAT2 txtScale = { 1.f, 1.f };

	float alphaButton = 0.15f;
	float alphaTxt = 1.f;
	XMFLOAT4 color = { 0.7f, 0.8f, 1.f, alphaButton };

	UIObject* p_bg = CreateGameObject<UIObject>();
	p_bg->CreateSprite(winCenter, winSize, { 0, 0, 0, 0.5f }, -1);
	p_bg->mp_sprite->SetDrawLayer(-1);


	UIObject* p_pause = CreateGameObject<UIObject>();
	{
		p_pause->CreateText({ winCenter.x, winCenter.y }, "Pause", 5, {0.5f, 0, 0, 0.75f}, (int)TagTexture::_FontSpooky, "Spooky");
		p_pause->mp_text->SetDrawLayer(-2);
	}

	Button* leave = CreateGameObject<Button>();
	{
		leave->SetProperties({ winSize.x - 75.f, 50.f }, buttonSize, color, -1, -1,
			[&]()
			{
				SCENE_MANAGER.SetExclusiveActiveScene((int)TagScene::_MainMenu);
			});
		leave->AddText("Quitter", txtScale, { 1, 1, 1, alphaTxt });
		leave->mp_sprite->SetDrawLayer(-2);
	}

	Button* options = CreateGameObject<Button>();
	{
		options->SetProperties({ winSize.x - 75.f, winSize.y - 50.f }, buttonSize, color, -1, -1,
			[&]()
			{
				SCENE_MANAGER.SetActiveScene((int)TagScene::_Options, true);
				SCENE_MANAGER.SetActiveScene((int)TagScene::_Pause, false);
			});
		options->AddText("Options", txtScale, { 1, 1, 1, alphaTxt });
		options->mp_sprite->SetDrawLayer(-2);
	}
}

void PauseScene::Start()
{
	StartGameObjects();
}

void PauseScene::Update()
{

}

void PauseScene::End()
{

}