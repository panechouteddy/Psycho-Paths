#pragma once
#include "ViewId.h"
namespace nam
{
	template<typename... Components>
	u32 ViewId::Get()
	{
		static u32 s_id = NextId();
		return s_id;
	}
}