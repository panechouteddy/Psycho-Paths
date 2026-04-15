#include "Text.h"
#include "pch.h"
#include "Text.h"

#include "RenderManager.h"
#include "CommandManager.h"

#include "Constants.h"

namespace nam
{
	Text::Text()
	{
		m_type = RenderItemType::_Text;
		m_psoToUse = PsoType::_Render2D;
	}

	void Text::SetDrawLayer(int drawLayer)
	{
		m_drawLayer = drawLayer;
	}

	void Text::SetText(std::string txt)
	{
		m_dirty = true;
		m_currentText = txt;

		m_vertices.clear();
		m_indices.clear();

		Font* p_font = FontManager::GetFont(m_fontName);

		if (p_font == nullptr)
			return;

		float penX = 0.f;
		float penY = 0.f;

		for (char c : txt)
		{
			if (c == '\n')
			{
				penX = 0.f;
				penY += p_font->m_lineHeight;;
				continue;
			}

			const Glyph& g = p_font->m_glyphs[c];

			XMFLOAT2 finalScale = { m_characterSize.x * m_inScale, m_characterSize.y * m_inScale };

			float px = (penX + g.xOffset) * finalScale.x;
			float py = (penY + g.yOffset) * finalScale.y;

			float x0 = px;
			float y0 = py;

			float x1 = px + g.width * finalScale.x;
			float y1 = py + g.height * finalScale.y;

			u32 indicesOffset = (u32)m_vertices.size();

			m_vertices.push_back({ {x0, y0, 0}, m_color, {g.u0, g.v0} });
			m_vertices.push_back({ {x0, y1, 0}, m_color, {g.u0, g.v1} });
			m_vertices.push_back({ {x1, y1, 0}, m_color, {g.u1, g.v1} });
			m_vertices.push_back({ {x1, y0, 0}, m_color, {g.u1, g.v0} });

			m_indices.push_back(indicesOffset);
			m_indices.push_back(indicesOffset + 1);
			m_indices.push_back(indicesOffset + 2);

			m_indices.push_back(indicesOffset + 2);
			m_indices.push_back(indicesOffset + 3);
			m_indices.push_back(indicesOffset + 0);

			penX += g.xAdvance * m_kerning;
		}
	}

	void Text::SetDatas(XMFLOAT2 characterSize, XMFLOAT4 textColor, int textureTag, std::string fontName)
	{
		m_fontName = fontName;

		m_dataSetted = true;
		m_characterSize = characterSize;
		m_color = textColor;

		SetTexture(textureTag);
	}

	const DirectX::XMFLOAT2& Text::GetCharacterSize() const
	{
		return m_characterSize;
	}

	const float& Text::GetKerning() const
	{
		return m_kerning;
	}

	XMFLOAT2 Text::GetOffsetsToCenter() const
	{
		if (m_vertices.size() == 0)
			return {0, 0};

		XMFLOAT3 minBounds = {};
		XMFLOAT3 maxBounds = {};

		XMFLOAT3 pos0 = m_vertices[0].m_pos;
		maxBounds.x = pos0.x;
		maxBounds.y = pos0.y;
		minBounds.x = pos0.x;
		minBounds.y = pos0.y;

		for (size_t i = 1; i < m_vertices.size(); i++)
		{
			XMFLOAT3 pos = m_vertices[i].m_pos;

			if (pos.x > maxBounds.x)
				maxBounds.x = pos.x;
			if (pos.y > maxBounds.y)
				maxBounds.y = pos.y;

			if (pos.x < minBounds.x)
				minBounds.x = pos.x;
			if (pos.y < minBounds.y)
				minBounds.y = pos.y;
		}

		return XMFLOAT2((minBounds.x + maxBounds.x) * 0.5f, (minBounds.x + maxBounds.y) * 0.5f);
	}

	void Text::SetKerning(float kerning)
	{
		m_kerning = kerning;
		SetText(m_currentText);
	}

	void Text::SetCharacterSize(DirectX::XMFLOAT2 size)
	{
		m_characterSize = size;
		SetText(m_currentText);
	}

	void Text::SetOpacity(float val)
	{
		m_color = { m_color.x, m_color.y, m_color.z, val };
		SetText(m_currentText);
	}
}
