#pragma once
#include <BaseTypes.h>
#include <ContainerHash.h>

namespace nam
{
	struct Glyph
	{
		float u0, v0;
		float u1, v1;

		float width, height;
		float xOffset, yOffset;
		float xAdvance;
	};

	struct Font
	{
		float m_lineHeight = 0.0f;
		float m_scaleW = 1.0f;
		float m_scaleH = 1.0f;

		UnMap<int, Glyph> m_glyphs;
	};

	//Download BMFont to create fonts
	//Default Params in the engine
	//Char size = 128 | Texture = 1024 X 1024
	//Export to dds
	struct FontManager
	{
		static UnMap<std::string, Font> m_fonts;

	public:
		static void LoadFnt(std::string name, std::string path);
		static Font* GetFont(std::string name);
	};
}

