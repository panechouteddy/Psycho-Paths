#pragma once

namespace nam
{
	struct Entity
	{
	public:
		u32 m_id;
		u8 m_generation;

	public:
		Entity();
		Entity(u32 id, u8 gen);

		bool operator==(const Entity& other) const;
		bool operator!=(const Entity& other) const;
	};
}