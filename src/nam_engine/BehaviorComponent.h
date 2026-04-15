#pragma once
namespace nam
{
	struct BehaviorComponent
	{
		std::function<void()> Update;
	};
}
