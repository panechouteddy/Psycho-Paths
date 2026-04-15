#include "pch.h"
#include "Chunk.h"

#include "Room.h"

Chunk::Chunk(Scene* p_owner)
{
	mp_owner = p_owner;
	m_chunkMin = {};
	m_chunkMax = {};
}

Chunk::~Chunk()
{
	UnLoad();
}

void Chunk::UnLoad()
{
	for (auto& [tag, room] : m_rooms)
	{
		delete room;
	}

	m_rooms.clear();
}

Room* Chunk::AddRoom(size tag, Room* p_room)
{
	auto it = m_rooms.find(tag);

	if (it != m_rooms.end())
	{
		_ASSERT(false && "A Room has already this tag");
		return nullptr;
	}

	m_rooms[tag] = p_room;

	return p_room;
}

void Chunk::SetActive(bool state)
{
	for (auto& [key, room] : m_rooms)
	{
		state ? room->Activate() : room->Desactivate();
	}
}

void Chunk::ActivateRoom(size roomTag)
{
	auto it = m_rooms.find(roomTag);

	if (it == m_rooms.end())
	{
		_ASSERT(false && "ActivateRoom() -> RoomTag not found !");
		return;
	}

	it->second->Activate();
}

void Chunk::DesactivateRoom(size roomTag)
{
	auto it = m_rooms.find(roomTag);

	if (it == m_rooms.end())
	{
		_ASSERT(false && "DesactivateRoom() -> RoomTag not found !");
		return;
	}

	it->second->Desactivate();
}

void Chunk::ToggleRoom(size roomTag)
{
	auto it = m_rooms.find(roomTag);

	if (it == m_rooms.end())
	{
		_ASSERT(false && "DesactivateRoom() -> RoomTag not found !");
		return;
	}

	Room* p_room = it->second;

	if (p_room->m_isActive)
		p_room->Desactivate();
	else
		p_room->Activate();
}

Room* Chunk::GetRoom(size roomTag)
{
	auto it = m_rooms.find(roomTag);

	if (it == m_rooms.end())
		return nullptr;

	return it->second;
}

UnMap<size, Room*>& Chunk::GetAllRooms()
{
	
	return m_rooms;
}

void Chunk::SetChunkIndex(GameObject* p_go)
{
	XMVECTOR minBoundsV = XMLoadFloat3(&p_go->GetComponent<MeshRendererComponent>().mp_mesh->m_minBounds);
	XMVECTOR maxBoundsV = XMLoadFloat3(&p_go->GetComponent<MeshRendererComponent>().mp_mesh->m_maxBounds);

	XMFLOAT3 position = p_go->mp_transform->GetWorldPosition();
	XMVECTOR centerV = XMLoadFloat3(&position);

	XMVECTOR goMinV = centerV + minBoundsV;
	XMFLOAT3 goMin;
	XMStoreFloat3(&goMin, goMinV);

	XMVECTOR goMaxV = centerV + maxBoundsV;
	XMFLOAT3 goMax;
	XMStoreFloat3(&goMax, goMaxV);

	if (goMin.x < m_chunkMin.x)
		m_chunkMin.x = goMin.x;
	if (goMin.z < m_chunkMin.z)
		m_chunkMin.z = goMin.z;

	if (goMax.x > m_chunkMax.x)
		m_chunkMax.x = goMax.x;
	if (goMax.z > m_chunkMax.z)
		m_chunkMax.z = goMax.z;
}

const size Chunk::GetRoomsCount() const
{
	return m_rooms.size();
}

const XMFLOAT3& Chunk::GetchunkMinIndex() const
{
	return m_chunkMin;
}

const XMFLOAT3& Chunk::GetchunkMaxIndex() const
{
	return m_chunkMax;
}
