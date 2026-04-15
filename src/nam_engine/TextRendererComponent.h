#pragma once
#include <Text.h>

namespace nam
{
	struct TextRendererComponent
	{
		Text* mp_text = nullptr;

	public:
		//Create a new Text in the component
		Text* CreateTextInstance();

		//Using a existing Text
		void SetTextInstance(Text* p_text);

		//Delete the Text member
		void DeleteTextInstance();
	};
}

