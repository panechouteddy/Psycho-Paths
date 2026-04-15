#pragma once
#include <Scene.h>
#include <AllTypes.h>

struct MiniScene
{
	Vector<GameObject*> m_objects;

public:
	void SetActive(bool state);
};

class OptionsScene : public Scene
{
	enum MiniSceneType
	{
		_General,
		_ForKeyboard,
		_ForController
	};

	//Keyboard | Controller
	UnMap<int, MiniScene> m_scenes = {};
	int m_currentConfig = -1;

	float m_scrollValue = 0.f;

	bool m_cameFromGame = false;

	int m_lastKeyPressed = -1;
	int m_lastButtonPressed = -1;

//Constants
	const float alphaTxt = 0.75f;
	const XMFLOAT2 winCenter = WINDOW->GetCenter(true);
	const XMFLOAT2 winSize = WINDOW->GetSize(true);
	const XMFLOAT4 color = { 0.7f, 0.8f, 1.f, 0.15f };
public:
	void Init();
	void Start();
	void Update();
	void End();

	void ActivateMiniScene(int type);

private:
	void LoadAssets();
	void InitKeyboardMiniScene(MiniScene& s);
	void InitControllerMiniScene(MiniScene& s);

	void InitGeneralMiniScene(MiniScene& s);
	void CreateSensivitySection(MiniScene& s, float offsetY, XMFLOAT2 buttonSize, XMFLOAT2 txtScale);
	void CreateGlobalVolumeSection(MiniScene& s, float offsetY, XMFLOAT2 buttonSize, XMFLOAT2 txtScale);
	void CreateMusicVolumeSection(MiniScene& s, float offsetY, XMFLOAT2 buttonSize, XMFLOAT2 txtScale);
	void CreateSfxVolumeSection(MiniScene& s, float offsetY, XMFLOAT2 buttonSize, XMFLOAT2 txtScale);

	std::string GetTextDisplay(std::string txt, float value, int decimals);

	void InitGlobalScene();
	void CreateUpperGlobalObjects(XMFLOAT2 txtScale, XMFLOAT4 color);
	void CreateBottomGlobalObjects(XMFLOAT2 txtScale, XMFLOAT4 color);
};

