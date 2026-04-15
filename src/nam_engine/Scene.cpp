#include "pch.h"
#include "Scene.h"

namespace nam
{
	void Scene::StartGameObjects()
	{
		for (auto it = m_gameObjects.begin(); it != m_gameObjects.end(); it++)
		{
			it->second->OnStart();
		}
	}

	void Scene::SetActiveAllGameObjects(bool state)
	{
		for (auto it = m_gameObjects.begin(); it != m_gameObjects.end(); it++)
		{
			GameObject* gameObject = it->second;
			gameObject->m_isActive = state;

			ECS.SetEntityActive(*gameObject->GetEntity(), state);
		}
	}

	void Scene::EndGameObjects()
	{
		for (auto it = m_gameObjects.begin(); it != m_gameObjects.end(); it++)
		{
			it->second->OnEnd();
		}
	}

	void Scene::OnDestroy()
	{
		DestroyAllGameObjects();
	}

	void Scene::HandleDestroy()
	{
		for (auto it = m_gameObjectsToDestroy.begin(); it != m_gameObjectsToDestroy.end(); it++)
		{
			GameObject* gameObject = it->second;
			int idEntity = gameObject->GetEntity()->m_id;

			ECS.DestroyEntity(*gameObject->GetEntity());
			m_gameObjects.erase(idEntity);

			delete gameObject;
		}

		m_gameObjectsToDestroy.clear();
	}

	void Scene::UpdateTransformComps()
	{
		for (auto it = m_gameObjects.begin(); it != m_gameObjects.end(); it++)
		{
			GameObject* gameObject = it->second;
			gameObject->mp_transform = &gameObject->GetComponent<TransformComponent>();
		}
	}

	void Scene::DestroyGameObject(GameObject* gameObject)
	{
		u32 idEntity = gameObject->GetEntity()->m_id;
		auto it = m_gameObjectsToDestroy.find(idEntity);

		if (it == m_gameObjectsToDestroy.end())
		{
			m_gameObjectsToDestroy[idEntity] = gameObject;
		}
	}

	void Scene::SetActive(bool active)
	{
		SetActiveAllGameObjects(active);

		if (m_isActive != active) // If the state changes
		{
			(active) ? Start() : End();
		}

		m_isActive = active;
	}

	//protected

	void Scene::DestroyAllGameObjects()
	{
		for (auto it = m_gameObjects.begin(); it != m_gameObjects.end(); it++)
		{
			DestroyGameObject(it->second);
		}
	}

	//public

	GameObject* Scene::GetFirstGameObjectWithTag(int tag)
	{
		for (auto& [key, go] : m_gameObjects)
		{
			if (go->GetTag() == tag)
				return go;
		}

		return nullptr;
	}

	Vector<GameObject*> Scene::GetAllGameObjectsWithTag(Vector<int> tags, bool ignoreDesactivatedObjs)
	{
		Vector<GameObject*> toGet;

		for (auto& [key, go] : m_gameObjects)
		{
			if (ignoreDesactivatedObjs && go->m_isActive == false)
				continue;

			for (int tag : tags)
			{
				if (go->GetTag() == tag)
					toGet.push_back(go);
			}
		}

		return toGet;
	}

	GameObject* Scene::GetGameObject(u32 id) const
	{
		auto it = m_gameObjects.find(id);

		if (it == m_gameObjects.end())
		{
			return nullptr;
		}

		return it->second;
	}

	const Vector<GameObject*> Scene::GetAllGameObjects() const
	{
		Vector<GameObject*> toGet;

		for (auto& [key, go] : m_gameObjects)
		{
			toGet.push_back(go);
		}

		return toGet;
	}

	const Vector<GameObject*> Scene::GetAllActiveGameObjects() const
	{
		Vector<GameObject*> toGet;

		for (auto& [key, go] : m_gameObjects)
		{
			if (go->m_isActive)
				toGet.push_back(go);
		}

		return toGet;
	}

	const bool Scene::IsActive() const
	{
		return m_isActive;
	}

	const size& Scene::GetTag() const
	{
		return m_tag;
	}
}


