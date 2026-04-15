#pragma once
#include "ARenderItem.h"

namespace nam
{
	struct Sprite : public ARenderItem
	{
		int m_drawLayer = 0;

		DirectX::XMFLOAT2 m_size = {0,0};

	public:
		Sprite();
		~Sprite() = default;

		//Works for Sprite & Text
		//ex : Sprite 1 is in front of Sprite 2
		//if Sprite 1 has a lower Layer than Sprite 2
		//Layer N is in front of Layer (N + 1)
		void SetDrawLayer(int drawLayer);

		//If erase == true, the vertices and indices will be cleared before
		void BuildRect(DirectX::XMFLOAT2 size, DirectX::XMFLOAT4 color, bool erase = false, DirectX::XMFLOAT2 offset = { 0, 0 }, float roll = 0);
		void BuildCircle(float radius, int stacks, DirectX::XMFLOAT4 color, DirectX::XMFLOAT2 offset = { 0, 0 });

		void SetOpacity(float value);
	};
}

