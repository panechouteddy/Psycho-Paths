#pragma once
#include "ARenderItem.h"
#include <BaseTypes.h>

#include "FontManager.h"

constexpr int MAX_CHARS = 1024;

constexpr int VERTICES_PER_CHAR = 4;
constexpr int INDICES_PER_CHAR = 6;

constexpr int MAX_TEXT_VERTICES = MAX_CHARS * VERTICES_PER_CHAR;
constexpr int MAX_TEXT_INDICES = MAX_CHARS * INDICES_PER_CHAR;

namespace nam
{
	struct Text : public ARenderItem
	{
		int m_drawLayer = 0; 

		std::string m_currentText = {};

		std::string m_fontName = "";

	private:
		bool m_dataSetted = false;
		float m_kerning = 1.f;

		DirectX::XMFLOAT2 m_characterSize = { 1, 1 };

		//To make a default 32px character size
		float m_inScale = 0.25f;
	public:
		Text();

		//Works for Sprite & Text (which is a sprite)
		//ex : Sprite 1 is in front of Sprite 2
		//if Sprite 1 has a lower Layer than Sprite 2
		//Layer N is in front of Layer (N + 1)
		void SetDrawLayer(int drawLayer);

		void SetText(std::string txt);
		void SetDatas(DirectX::XMFLOAT2 characterSize, DirectX::XMFLOAT4 textColor = { 1, 1, 1, 1 }, int textureTag = _FontPusab, std::string fontName = "Pusab");

		const DirectX::XMFLOAT2& GetCharacterSize() const;
		const float& GetKerning() const;

		XMFLOAT2 GetOffsetsToCenter() const;

		void SetKerning(float kerning);
		void SetCharacterSize(DirectX::XMFLOAT2 size);
		void SetOpacity(float val);
	};
}

