#include "pch.h"
#include "ViewId.h"

namespace nam
{
	u32 ViewId::ms_lastId = 0;

	u32 ViewId::NextId()
	{
		return ms_lastId++;
	}

	u32 ViewId::GetCurrentLast()
	{
		return ms_lastId;
	}
}