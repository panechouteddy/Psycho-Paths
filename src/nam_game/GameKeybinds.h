#pragma once
#include <AllTypes.h>

constexpr const char* PATH_BINDS = "../../res/Save/keyconfigs.txt";

namespace KeyAction
{
	constexpr const char* _MoveLeft = "Gauche";
	constexpr const char* _MoveRight = "Droite";
	constexpr const char* _MoveUp = "Haut";
	constexpr const char* _MoveDown = "Bas";

	constexpr const char* _Jump = "Sauter";
	constexpr const char* _Crouch = "S'accroupir";
	constexpr const char* _Sprint = "Sprinter";
	constexpr const char* _ToggleLight = "Allumer(Lumiere)";
	constexpr const char* _ReloadLight = "Recharger(Lumiere)";

	constexpr const char* _Pause = "Pause";
	constexpr const char* _Interact = "Interagir";
	constexpr const char* _Inventory = "Inventaire";
};

struct BindData
{
	std::string name;
	int keyboardKey;
	int controllerKey;
};

struct GameKeybinds
{
	//first = keyboard | second = controller
	static UnMap<std::string, std::pair<int, int>> m_keybinds;

private:
	static int m_currentBindCount;
	static UnMap<int, std::string> m_bindsOrder;

public:
	static void InitFromFile();
	static void SaveBindsInFile();

	static Vector<BindData> GetAllBindsData();

	static int GetKey(std::string name);
	static KeyButton GetButton(std::string name);
private: 
	static void AddKeybind(std::string actionName, std::pair<int, int> bind);
	static void SetDefaultKeybinds();
};

