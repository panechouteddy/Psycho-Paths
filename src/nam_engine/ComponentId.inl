#pragma once 

namespace nam
{
	template<typename Component>
	u32 ComponentId::Get()
	{
		static u32 s_id = NextId();
		return s_id;
	}
}