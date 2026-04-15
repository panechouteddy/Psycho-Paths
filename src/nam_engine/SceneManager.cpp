#include "pch.h"
#include "SceneManager.h"
#include "App.h"

namespace nam
{
	SceneManager::SceneManager()
	{

	}

	void SceneManager::Init()
	{
		Reset();
	}

	void SceneManager::Update()
	{
		for (auto& [tag, scene] : m_scenes)
		{
			if (scene->IsActive() == false)
				continue;

			scene->Update();
			scene->HandleDestroy();
		}
	}

	void SceneManager::Destroy()
	{
		for (auto& [tag, scene] : m_scenes)
		{
			delete scene;
		}

		m_scenes.clear();
	}

	void SceneManager::Reset()
	{
		Destroy();
		Init();
	}

	void SceneManager::SetExclusiveActiveScene(size sceneTag)
	{
		for (auto& [key, scene] : m_scenes)
		{
			if (scene->IsActive() == false)
				continue;

			scene->SetActive(false);
		}

		GetScene(sceneTag)->SetActive(true);
	}

	void SceneManager::SetActiveScene(size sceneTag, bool state)
	{
		GetScene(sceneTag)->SetActive(state);
	}

	Scene* SceneManager::CreateScene(size sceneTag)
	{
		auto it = m_scenes.find(sceneTag);

		if (it != m_scenes.end())
		{
			_ASSERT(false && "SceneManager has already a Scene with this tag !");
			return nullptr;
		}

		Scene* scene = new Scene();
		scene->m_tag = sceneTag;

		m_scenes[sceneTag] = scene;

		return scene;
	}

	void SceneManager::DestroyScene(size sceneTag)
	{
		auto it = m_scenes.find(sceneTag);

		if (it == m_scenes.end())
		{
			_ASSERT(false && "Scene not found ! Can't Destroy");
			return;
		}

		delete it->second;
		m_scenes.erase(it);
	}

	bool SceneManager::IsSceneActive(size sceneTag)
	{
		auto it = m_scenes.find(sceneTag);

		if (it == m_scenes.end())
		{
			_ASSERT(false && "Scene not found !");
			return false;
		}

		return it->second->IsActive();
	}

//public

	Scene* SceneManager::GetScene(size sceneTag)
	{
		auto it = m_scenes.find(sceneTag);

		if (it == m_scenes.end())
		{
			_ASSERT(false && "Scene not found ! Can't Get");
			return nullptr;
		}

		return it->second;
	}

	SceneManager::~SceneManager()
	{
		Destroy();
	}
}



