#pragma once
#include <ContainerSequential.h>
#include <json.hpp>

namespace nam
{
	class GameObject;
}

class Chunk;
class GameScene;

namespace Regexword
{
	//Names with variables
	constexpr const char* MATERIAL = "M([0-9]+)";
	constexpr const char* ROOM = "R([0-9]+)";

	//for Monster behavior
	constexpr const char* CHECKPOINT = "cp";
	constexpr const char* PURSUITPOINT = "pp([0-9]+)";

	//Specific classes
	constexpr const char* FLASHLIGHT = "flashlight";
	constexpr const char* FIRST_KEY = "firstkey";
	constexpr const char* LOCKER = "Locker";
	constexpr const char* EQUATION_SHEET = "equationsheet";
	constexpr const char* SAVE = "save";
	constexpr const char* DOOR = "door";
	constexpr const char* BATTERY = "battery";
	constexpr const char* ENDSCREEN = "endscreen";
	constexpr const char* DIGICODE = "digicode([0-9]+)";

	//Specific behaviors
	constexpr const char* LIGHT = "light";
	constexpr const char* TRIGGER_PURSUIT = "triggerpursuit";
	constexpr const char* COLLIDER = "P";
	constexpr const char* NOREPULSE = "NR";
	constexpr const char* NOPATH = "nopath";

	constexpr const char* NO_MESH = "nomesh";

	//Tags to set
	constexpr const char* FLOOR = "floor";
	constexpr const char* WALL = "wall";
}

//Specific to Game
class BlenderSceneCustom
{
public:
	static Vector<GameObject*> Load(std::wstring jsonFile, size tagScene, std::wstring textureFolder, Chunk* p_chunk);

private:
	static GameObject* HandleGameObjectType(Scene* s, std::string name);

	static void HandleTransform(nlohmann::json::value_type& obj, GameObject* p_go);
	static void HandleMaterial(std::string name, Mesh* p_mesh);
	static void HandleTexture(nlohmann::json::value_type& obj, Mesh* p_mesh, std::wstring textureFolder);

	static Mesh* HandleMesh(nlohmann::json::value_type& obj, std::string name, GameObject* p_go, XMFLOAT3& boundingBox);
	static void HandleCollider(std::string name, GameObject* p_go, const XMFLOAT3& boundingBox);

	static void HandleTagFloor(std::string name, GameObject* p_go);
	static void HandleTagWall(std::string name, GameObject* p_go);

	static void HandleLight(std::string name, GameObject* p_go);
	static void HandlePathCheckpoint(GameScene* gs, std::string name, GameObject*& p_go);
	static void HandlePursuitPoint(GameScene* gs, std::string name, GameObject* p_go);

	static void HandleChunkRoom(Chunk* p_chunk, Scene* p_scene, std::string name, GameObject* p_go);
};

