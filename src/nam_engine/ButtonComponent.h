#pragma once
namespace nam
{
	struct ButtonComponent
	{
		bool m_isHovered = false;
		std::function<void()> OnClick;
		std::function<void()> OnHovered;
		std::function<void()> OnLeft;
	};
}
