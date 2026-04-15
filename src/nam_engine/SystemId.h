#pragma once

namespace nam
{
	class SystemId
	{
	private:
		static u32 ms_lastId;

	private:
		static u32 NextId();

	public:
		template<typename System>
		static u32 Get();
		static u32 GetCurrentLast();
	};
}
#include "SystemId.inl"