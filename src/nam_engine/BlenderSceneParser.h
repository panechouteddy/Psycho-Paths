#pragma once
#include <ContainerSequential.h>
#include <json.hpp>

namespace nam
{
	class GameObject;

	class BlenderSceneParser
	{
	public:
		static Vector<GameObject*> Load(std::wstring jsonFile, size tagScene, std::wstring textureFolder);

	private:
		static void HandleTransform(nlohmann::json::value_type& obj, GameObject* p_go);
		static void HandleMaterial(std::string name, Mesh* p_mesh);
		static void HandleTexture(nlohmann::json::value_type& obj, Mesh* p_mesh, std::wstring textureFolder);

		static Mesh* HandleMesh(nlohmann::json::value_type& obj, GameObject* p_go);

		static void HandleCollider(std::string name, GameObject* p_go);
	};
}

