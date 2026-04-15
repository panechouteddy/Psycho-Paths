#pragma once
namespace nam
{
	class ViewId
	{
	private:
		static u32 ms_lastId;

	private:
		static u32 NextId();

	public:
		template<typename... Components>
		static u32 Get();
		static u32 GetCurrentLast();
	};
}
#include "ViewId.inl"
