#include "pch.h"
#include "Room.h"

Room::Room(Scene* p_owner)
{
	mp_owner = p_owner;
}

Room::~Room()
{
	UnLoad();
}

void Room::UnLoad()
{
	for (GameObject* p_go : m_roomObjects)
	{
		p_go->Destroy();
	}

	m_roomObjects.clear();
}

void Room::AddGameObject(GameObject* p_go)
{
	m_roomObjects.push_back(p_go);
}

void Room::AddGameObjects(Vector<GameObject*> p_GOs)
{
	for (GameObject* p_go : p_GOs)
	{
		m_roomObjects.push_back(p_go);
	}
}

Vector<GameObject*>& Room::GetGameObjects()
{
	return m_roomObjects;
}

void Room::Activate()
{
	for (GameObject* p_go : m_roomObjects)
	{
		p_go->SetActive(true);
	}

	m_isActive = true;
}

void Room::Desactivate()
{
	for (GameObject* p_go : m_roomObjects)
	{
		p_go->SetActive(false);
	}

	m_isActive = false;
}
