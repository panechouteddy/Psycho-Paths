#include "pch.h"
#include "GameKeybinds.h"
#include <GamePad.h>
#include "GeneralConfigs.h"

UnMap<std::string, std::pair<int, int>> GameKeybinds::m_keybinds = {};

int GameKeybinds::m_currentBindCount = 0;
UnMap<int, std::string> GameKeybinds::m_bindsOrder = {};

void GameKeybinds::InitFromFile()
{
	std::ifstream file(PATH_BINDS);

	if (!file.is_open())
	{
		SetDefaultKeybinds();
		SaveBindsInFile();
	}
	else
	{
		std::string line;
		while (std::getline(file, line))
		{
			std::istringstream ss(line);
			std::string prefix;

			ss >> prefix;

			if (prefix.empty())
				continue;

			int keybind = -1;
			int controllerBind = -1;

			ss >> keybind;
			ss >> controllerBind;

			AddKeybind(prefix, { keybind, controllerBind });
		}
	}
}

void GameKeybinds::SaveBindsInFile()
{
	std::ofstream fileOut(PATH_BINDS, std::ios::trunc);

	if (!fileOut.is_open())
		return;

	for (BindData& data : GetAllBindsData())
	{
		fileOut << data.name << " " << data.keyboardKey << " " << data.controllerKey << "\n";
	}

	fileOut.close();

	Print("Keybinds saved !");
}

Vector<BindData> GameKeybinds::GetAllBindsData()
{
	Vector<BindData> datas;

	for (int i = 0; i < m_currentBindCount; i++)
	{
		std::string name = m_bindsOrder[i];
		auto& bind = m_keybinds[name];

		datas.push_back(BindData{ name, bind.first, bind.second });
	}

	return datas;
}

int GameKeybinds::GetKey(std::string name)
{
	auto it = m_keybinds.find(name);

	if (it == m_keybinds.end())
		return -1;

	return it->second.first;
}

KeyButton GameKeybinds::GetButton(std::string name)
{
	auto it = m_keybinds.find(name);

	if (it == m_keybinds.end())
		return KeyButton::COUNT;

	return KeyButton(it->second.second);
}

void GameKeybinds::AddKeybind(std::string actionName, std::pair<int, int> bind)
{
	m_keybinds[actionName] = bind;
	m_bindsOrder[m_currentBindCount] = actionName;

	m_currentBindCount++;
}

void GameKeybinds::SetDefaultKeybinds()
{
	AddKeybind(KeyAction::_MoveLeft, { 'Q', -1 });
	AddKeybind(KeyAction::_MoveRight, { 'D', -1 });
	AddKeybind(KeyAction::_MoveUp, { 'Z', -1 });
	AddKeybind(KeyAction::_MoveDown, { 'S', -1 });

	AddKeybind(KeyAction::_Jump, { VK_SPACE, KeyButton::A });
	AddKeybind(KeyAction::_Crouch, { VK_LCONTROL, KeyButton::RThumb });
	AddKeybind(KeyAction::_Sprint, { VK_LSHIFT, KeyButton::LThumb });
	AddKeybind(KeyAction::_ToggleLight, { 'F', KeyButton::RB});
	AddKeybind(KeyAction::_ReloadLight, { 'R', KeyButton::X });

	AddKeybind(KeyAction::_Pause, { VK_ESCAPE, KeyButton::Start });
	AddKeybind(KeyAction::_Interact, { 'E', KeyButton::B });
	AddKeybind(KeyAction::_Inventory, { 'I', KeyButton::Y });
}
