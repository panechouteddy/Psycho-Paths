#pragma once
#include "App.h"

namespace nam
{
	class SceneManager;
	class GameObject;

	class Scene
	{
	protected:
		UnMap<u32, GameObject*> m_gameObjects;
		UnMap<u32, GameObject*> m_gameObjectsToDestroy;

		size m_tag = -1;

		bool m_isActive = false;

	public:
		Scene() = default;
		~Scene() = default;

		//Called at the Creation
		virtual void Init() {};

		//Called when the scene is Activated
		virtual void Start() {};
		virtual void Update() {};
		//Called when the scene is Desactivated
		virtual void End() {};

		void OnDestroy();

		//Call the Start of All gameObjects
		void StartGameObjects();
		void SetActiveAllGameObjects(bool state);

		//Call the End of All gameObjects
		void EndGameObjects();

		template<typename T>
		T* CreateGameObject();

		void DestroyGameObject(GameObject* gameObject);

		void SetActive(bool active);

		void DestroyAllGameObjects();

	public:
		GameObject* GetFirstGameObjectWithTag(int tag);
		Vector<GameObject*> GetAllGameObjectsWithTag(Vector<int> tags, bool ignoreDesactivatedObjs = false);

		GameObject* GetGameObject(u32 id) const;

		const Vector<GameObject*> GetAllGameObjects() const;
		const Vector<GameObject*> GetAllActiveGameObjects() const;

		const bool IsActive() const;
		const size& GetTag() const;

	protected:
		void HandleDestroy();
		void UpdateTransformComps();

		friend class SceneManager;
		friend class GameObject;
	};
}
#include "Scene.inl"

