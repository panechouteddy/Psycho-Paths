#pragma once
#include <functional>

namespace nam 
{
	struct InteractComponent
	{
		std::string m_indicator = "o";
		XMFLOAT4 m_defaultColor = { 1, 1, 1, 0.75f };
		XMFLOAT4 m_indicatorColor = m_defaultColor;
		
		std::function<void()> OnTrigger = nullptr;

	public:
		void InitInteract(std::function<void()> onTrigger);
	};
}

