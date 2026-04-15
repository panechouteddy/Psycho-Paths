#pragma once
namespace nam
{
	class ComponentId
	{
	private:
		static u32 ms_lastId;

	private:
		static u32 NextId();

	public:
		static u32 GetCurrentLast();
		template<typename Component>
		static u32 Get();
	};
}

#include "ComponentId.inl"