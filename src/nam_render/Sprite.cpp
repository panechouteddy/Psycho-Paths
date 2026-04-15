#include "pch.h"
#include "Sprite.h"

namespace nam
{
	Sprite::Sprite()
	{
		m_type = RenderItemType::_Sprite;
		m_psoToUse = PsoType::_Render2D;
	}

	void Sprite::SetDrawLayer(int drawLayer)
	{
		m_drawLayer = drawLayer;
	}

	void Sprite::BuildRect(XMFLOAT2 size, XMFLOAT4 color, bool erase, XMFLOAT2 offset, float roll)
	{
		if (erase)
		{
			m_vertices.clear();
			m_indices.clear();
		}

		m_color = color;

		Vector<Vertex> vertices;
		Vector<u32> indices;

		XMFLOAT2 halfSize = { size.x * 0.5f, size.y * 0.5f };

		vertices =
		{
			Vertex{ XMFLOAT3(-halfSize.x, -halfSize.y, 0), color, {0, 0} },
			Vertex{ XMFLOAT3(-halfSize.x,  halfSize.y, 0), color, {0, 1} },
			Vertex{ XMFLOAT3(halfSize.x, halfSize.y, 0), color, {1, 1} },
			Vertex{ XMFLOAT3(halfSize.x, -halfSize.y, 0), color , { 1, 0 } },
		};

		indices =
		{
			0, 1, 2,
			2, 3, 0
		};
    
		m_size = size;

		AddToGlobalMesh(vertices, indices, { offset.x, offset.y, 0 }, { 0, 0, roll });
	}

	void Sprite::BuildCircle(float radius, int stacks, DirectX::XMFLOAT4 color, XMFLOAT2 offset)
	{
		if (stacks < 3)
			return;

		m_color = color;

		Vector<Vertex> vertices;
		Vector<u32> indices;

		float step = XM_2PI / stacks;

		for (int i = 0; i < stacks + 1; i++)
		{
			float angle = i * step;

			float x = cosf(angle) * radius;
			float y = sinf(angle) * radius;

			float u = (float)x * 0.5f + 0.5f;
			float v = (float)y * 0.5f + 0.5f;

			vertices.push_back(Vertex{ XMFLOAT3(x, y, 0), color, {u, v} });
		}

		uint32_t center = (uint32_t)vertices.size();
		vertices.push_back(Vertex{ XMFLOAT3(0, 0, 0), color });

		for (int i = 0; i < stacks; ++i)
		{
			uint32_t base = i;

			indices.push_back(center);
			indices.push_back(base + 1);
			indices.push_back(base);
		}

		m_size = { radius * 2.f, radius * 2.f };
		AddToGlobalMesh(vertices, indices, { offset.x, offset.y, 0 }, { 0, 0, 0 });
	}

	void Sprite::SetOpacity(float value)
	{
		SetColor({ m_color.x, m_color.y, m_color.z, value });
	}
}