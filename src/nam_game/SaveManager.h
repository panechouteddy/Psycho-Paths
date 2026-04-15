#pragma once
#include <DirectXMath.h>

constexpr const char* PATH_SAVE = "../../res/Save/save.txt";
constexpr const char* PATH_PLAYTIME = "../../res/Save/playtime.txt";

class UIObject;

struct InventoryDatas
{
	std::vector<std::pair<std::string, u8>> datas;
};

struct SaveDatas
{
	bool hasFlashlight = false;
	DirectX::XMFLOAT3 playerPos = { 15.4202f, 1, 0.6f };
	InventoryDatas inventory;
};

struct SaveManager
{
	static UIObject* mp_savingText;
	static SaveDatas m_saveDatas;
	static Timer m_displayTimer;

	static float m_playTime;
public:
	static void GetDatasFromFile();
	static void GetNewDatas();

	static SaveDatas& GetDatas();

	static void SaveDatasInFile();
	static void UpdateDisplay(float dt);

	static void GetPlaytimeFromFile();
	static void SavePlaytimeInFile();
};

