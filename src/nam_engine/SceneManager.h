#pragma once
#include <FrameworkCore.h>
namespace nam
{
	class Scene;

	class SceneManager final
	{
	private:
		UnMap<size, Scene*> m_scenes;

	public:
		SceneManager();

		void Init();
		void Update();
		void Destroy();

		void Reset();

		//Active this scene and desactivate the others
		void SetExclusiveActiveScene(size sceneTag);
		void SetActiveScene(size sceneTag, bool state);

		Scene* CreateScene(size sceneTag);

		template<typename T>
		T* CreateScene(size sceneTag);

		void DestroyScene(size sceneTag);

		bool IsSceneActive(size sceneTag);

	public:
		Scene* GetScene(size sceneTag);

		~SceneManager();

	private:
		friend class Scene;
	};
}

#include "SceneManager.inl"

