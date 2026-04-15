#ifndef PCH_H
#define PCH_H

//lib

#if _DEBUG
#pragma comment(lib, "../../ide/nam_engine/x64/Debug/nam_engine.lib")
#else
#pragma comment(lib, "../../ide/nam_engine/x64/Release/nam_engine.lib")
#endif

#include <FrameworkEngine.h>
#include <CollisionInfo.h>

using namespace nam;
using namespace DirectX;

enum class TagMaterial : size
{
	_METAL,
	_MONSTER,
	_ITEM,
};

enum class TagTexture : size
{
	_FontSen,
	_FontSpooky,
	_Wall,
	_Floor,
	_Mogwai,
	_Wood,
	_MenuBackground,
	_Inventory,
	_SprintBar,
	_BatteryCharge,
	_Stalker
};

enum class TagCollision : size
{
	_Decor,
};

enum class TagObject : size
{
	_Player,
	_Flashlight,
	_Camera,
	_Floor,
	_WallPaper,
	_WallIsolate,

	_NoiseEmitter,
	_Monster,
	
	_Button,
	_Box,

	_Locker,
	_Battery,

	_ClimbObject,

	_Door_Closed,
};

enum class TagScene : size
{
	_MainMenu,
	_Options,
	_Main,
	_Pause,
	_GameOver,
	_Inventory,
	_EndCreditsScene,
};

#include "FrameworkGame.h"

#endif  