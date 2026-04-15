#include "pch.h"
#include "SystemId.h"

namespace nam
{
	u32 SystemId::ms_lastId = 0;

	u32 SystemId::NextId()
	{
		assert(ms_lastId < EcsConfig::MAX_SYSTEMS && "MAX_SYSTEMS exceeded");
		return ms_lastId++;
	}

	u32 SystemId::GetCurrentLast()
	{
		return ms_lastId;
	}
}