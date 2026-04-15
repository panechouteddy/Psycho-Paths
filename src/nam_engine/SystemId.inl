#pragma once
#include "SystemId.h"
namespace nam
{
	template<typename System>
	u32 SystemId::Get()
	{
		static u32 s_id = NextId();
		return s_id;
	}
}