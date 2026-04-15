#include "pch.h"
#include "Entity.h"

namespace nam
{
	Entity::Entity() : m_id(-1), m_generation(-1)
	{

	}

	Entity::Entity(u32 id, u8 gen) : m_id(id), m_generation(gen)
	{

	}

	bool Entity::operator==(const Entity& other) const
	{
		return m_id == other.m_id && m_generation == other.m_generation;
	}

	bool Entity::operator!=(const Entity& other) const
	{
		return !(*this == other);
	}
}