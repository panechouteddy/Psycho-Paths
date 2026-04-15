#pragma once

namespace nam
{
	template<typename T>
	inline T* Scene::CreateGameObject()
	{
		static_assert(std::is_base_of<GameObject, T>::value, "T must derive from GameObject");

		T* gameObject = new T();

		Entity entity = ECS.CreateEntity();

		gameObject->Init(this, entity);
		gameObject->SetActive(false);

		int idEntity = entity.m_id;
		m_gameObjects[idEntity] = gameObject;

		return gameObject;
	}

}