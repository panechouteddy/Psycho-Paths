#include "pch.h"
#include "ComponentId.h"

namespace nam
{
	u32 ComponentId::ms_lastId = 0;

	u32 ComponentId::NextId()
	{
		assert(ms_lastId < EcsConfig::MAX_COMPONENTS && "MAX_COMPONENTS exceeded");
		return ms_lastId++;
	}

	u32 ComponentId::GetCurrentLast()
	{
		return ms_lastId;
	}
}