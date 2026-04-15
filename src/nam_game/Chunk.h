#pragma once
#include <ContainerAssociative.h>

namespace nam
{
	class Scene;
}

class Room;

class Chunk
{
protected:
	Scene* mp_owner = nullptr;
	XMFLOAT3 m_chunkMin;
	XMFLOAT3 m_chunkMax;
	UnMap<size, Room*> m_rooms;

	void UnLoad();

public:
	Chunk(Scene* p_owner);
	~Chunk();

	virtual void Load() {};

	Room* AddRoom(size tag, Room* p_room);

	void SetActive(bool state);

	void ActivateRoom(size roomTag);
	void DesactivateRoom(size roomTag);

	void ToggleRoom(size roomTag);

	Room* GetRoom(size roomTag);
	UnMap<size, Room*>& GetAllRooms();

	void  SetChunkIndex(GameObject* p_go);

	const size GetRoomsCount() const;
	const XMFLOAT3& GetchunkMinIndex() const;
	const XMFLOAT3& GetchunkMaxIndex() const;
};

