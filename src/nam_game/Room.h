#pragma once
#include <GameObject.h>

#include <ContainerSequential.h>

namespace nam
{
	class Scene;
}

class Room
{
protected:
	Scene* mp_owner = nullptr;
	Vector<GameObject*> m_roomObjects;

	void UnLoad();

public:
	bool m_isActive = false;

	Room(Scene* p_owner);
	~Room();

	virtual void Load() {};
	virtual void Update() {};

	void AddGameObject(GameObject* p_go);
	void AddGameObjects(Vector<GameObject*> p_GOs);
	Vector<GameObject*>& GetGameObjects();

	void Activate();
	void Desactivate();
};

