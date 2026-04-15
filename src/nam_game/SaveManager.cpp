#include "pch.h"
#include "SaveManager.h"
#include "UIObject.h"
#include <fstream>

UIObject* SaveManager::mp_savingText = nullptr;
SaveDatas SaveManager::m_saveDatas = {};
Timer SaveManager::m_displayTimer = {};

float SaveManager::m_playTime = 0.f;

void SaveManager::GetDatasFromFile()
{
	std::ifstream file(PATH_SAVE);

	if (!file.is_open())
	{
		_ASSERT(false && "File can't be open !\n");
		return;
	}

	m_saveDatas.inventory.datas.clear();

	std::string line;
	while (std::getline(file, line))
	{
		std::istringstream ss(line);
		std::string prefix;

		ss >> prefix;

		if (prefix == "Flashlight")
		{
			int val = -1;
			ss >> val;

			if (val > 0)
				m_saveDatas.hasFlashlight = true;
			else
				m_saveDatas.hasFlashlight = false;
		}

		if (prefix == "Position")
		{
			XMFLOAT3 pos = {};
			ss >> pos.x; ss >> pos.y; ss >> pos.z;

			m_saveDatas.playerPos = pos;
		}

		if (prefix == "Inventory")
		{
			std::string word = "START";
			ss >> word;

			while (word != "END")
			{
				std::string itemName = "";

				while (word != "|")
				{
					itemName += word + " ";
					ss >> word; // read next word of this item
				}

				itemName = itemName.substr(0, itemName.size() - 1);

				u8 itemAmount = 0;

				ss >> word; // skip the first |

				itemAmount = std::stoi(word);
				ss >> word; // to second |
				ss >> word; // to next ItemName

				m_saveDatas.inventory.datas.push_back({ itemName, itemAmount });
			}
		}
	}

	Print("Loading Save !");
}

void SaveManager::GetNewDatas()
{
	SaveDatas newDatas = {};
	m_saveDatas = newDatas;
	SaveDatasInFile();
}

SaveDatas& SaveManager::GetDatas()
{
	return m_saveDatas;
}

void SaveManager::SaveDatasInFile()
{
	SavePlaytimeInFile();

	if (mp_savingText == nullptr)
	{
		XMFLOAT2 winPos = WINDOW->GetCenter(true);

		mp_savingText = SCENE_MANAGER.GetScene((int)TagScene::_Main)->CreateGameObject<UIObject>();
		mp_savingText->CreateText({ winPos.x, winPos.y }, "Sauvegarde...", 1.5f, { 1, 1, 1 , 0.5f });
		mp_savingText->SetActive(false);

		m_displayTimer.Init(1.5f);
		m_displayTimer.Pause();
	}

	m_displayTimer.ResetProgress();
	m_displayTimer.Resume();

	std::ofstream fileOut(PATH_SAVE, std::ios::trunc);
	if (!fileOut.is_open())
		return;

	Player* p_player = (Player*)SCENE_MANAGER.GetScene((int)TagScene::_Main)->GetFirstGameObjectWithTag((int)TagObject::_Player);

	if (p_player == nullptr)
	{
		_ASSERT("Player nullptr in Save");
		return;
	}

	int hasFlash = p_player->HasFlashlight() ? 1 : 0;
	fileOut << "Flashlight " << hasFlash << "\n";

	XMFLOAT3 pos = p_player->mp_transform->GetWorldPosition();
	fileOut << "Position " << pos.x << " " << pos.y << " " << pos.z << "\n";

	auto& items = p_player->GetInventory().GetItems();

	fileOut << "Inventory ";
	for(auto& item : items)
	{
		fileOut << item.first << " | " << (int)item.second.second << " | ";
	} 
	fileOut << "END\n";

	fileOut.close();
	Print("Saving !");
}

void SaveManager::UpdateDisplay(float dt)
{
	if (mp_savingText == nullptr)
		return;

	if (m_displayTimer.IsPaused())
	{
		mp_savingText->SetActive(false);
		return;
	}
	else
		mp_savingText->SetActive(true);

	if (m_displayTimer.IsTargetReached())
		m_displayTimer.Pause();
	else
		m_displayTimer.Update(dt);
}

void SaveManager::GetPlaytimeFromFile()
{
	std::ifstream file(PATH_PLAYTIME);

	if (!file.is_open())
	{
		_ASSERT(false && "File can't be open !\n");
		return;
	}

	std::string line;
	while (std::getline(file, line))
	{
		std::istringstream ss(line);
		std::string prefix;

		ss >> prefix;

		if (prefix == "Playtime")
		{
			ss >> m_playTime;
		}
	}
}

void SaveManager::SavePlaytimeInFile()
{
	std::ofstream fileOut(PATH_PLAYTIME, std::ios::trunc);

	if (!fileOut.is_open())
		return;

	float currentSessionTime = (float)CHRONO.GetRealTotalTime();
	float totalSessionTime = m_playTime + currentSessionTime;

	fileOut << "Playtime " << totalSessionTime << "\n";
	CHRONO.Reset();
}
