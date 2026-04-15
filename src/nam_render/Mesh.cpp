#include "pch.h"
#include "Mesh.h"
#include "RenderManager.h"
#include "CommandManager.h"
#include "PerlinNoise.h"
#include "Common/DDSTextureLoader.h"
#include "VoxelGrid.h"
#include <PointHasher.h>

#include "ObjParser.h"
#include "GlbParser.h"

namespace nam
{
	Mesh::Mesh()
	{
		m_type = RenderItemType::_Mesh;
		m_psoToUse = PsoType::_Render3D;
	}

	Mesh::~Mesh()
	{
	}

	void Mesh::ComputeNormals(bool smoothShading)
	{
		for (size_t i = 0; i < m_indices.size(); i += 3)
		{
			u32 i0 = m_indices[i];
			u32 i1 = m_indices[i + 1];
			u32 i2 = m_indices[i + 2];

			XMVECTOR p0 = XMLoadFloat3(&m_vertices[i0].m_pos);
			XMVECTOR p1 = XMLoadFloat3(&m_vertices[i1].m_pos);
			XMVECTOR p2 = XMLoadFloat3(&m_vertices[i2].m_pos);

			XMVECTOR n = ComputeNormal(p0, p1, p2);

			if (smoothShading)
			{
				XMStoreFloat3(&m_vertices[i0].m_normal, XMLoadFloat3(&m_vertices[i0].m_normal) + n);
				XMStoreFloat3(&m_vertices[i1].m_normal, XMLoadFloat3(&m_vertices[i1].m_normal) + n);
				XMStoreFloat3(&m_vertices[i2].m_normal, XMLoadFloat3(&m_vertices[i2].m_normal) + n);
			}
			else
			{
				XMStoreFloat3(&m_vertices[i0].m_normal, n);
				XMStoreFloat3(&m_vertices[i1].m_normal, n);
				XMStoreFloat3(&m_vertices[i2].m_normal, n);
			}
		}

		if (smoothShading)
		{
			for (auto& v : m_vertices)
			{
				XMVECTOR normalizedNormal = XMVector3Normalize(XMLoadFloat3(&v.m_normal));
				XMStoreFloat3(&v.m_normal, normalizedNormal);
			}
		}
	}

	void Mesh::BuildCube(float size, XMFLOAT4 color, XMFLOAT3 offset, XMFLOAT3 ypr)
	{
		m_radius = -1;
		m_color = color;
		float halfSize = size * 0.5f;

		Vector<Vertex> vertices;
		Vector<u32> indices;

		auto addFace = [&](XMFLOAT3 p0, XMFLOAT3 p1, XMFLOAT3 p2, XMFLOAT3 p3, XMFLOAT3 normal)
			{
				u16 start = (u16)vertices.size();

				vertices.push_back({ p0, color, {0.f,1.f}, normal });
				vertices.push_back({ p1, color, {0.f,0.f}, normal });
				vertices.push_back({ p2, color, {1.f,0.f}, normal });
				vertices.push_back({ p3, color, {1.f,1.f}, normal });

				indices.push_back(start + 0);
				indices.push_back(start + 1);
				indices.push_back(start + 2);

				indices.push_back(start + 0);
				indices.push_back(start + 2);
				indices.push_back(start + 3);
			};

		// Front
		addFace(
			{ -halfSize,-halfSize,-halfSize },
			{ -halfSize, halfSize,-halfSize },
			{ halfSize, halfSize,-halfSize },
			{ halfSize,-halfSize,-halfSize },
			{ 0,0,-1 }
		);

		// Back
		addFace(
			{ -halfSize,-halfSize, halfSize },
			{ halfSize,-halfSize, halfSize },
			{ halfSize, halfSize, halfSize },
			{ -halfSize, halfSize, halfSize },
			{ 0,0,1 }
		);

		// Left
		addFace(
			{ -halfSize,-halfSize, halfSize },
			{ -halfSize, halfSize, halfSize },
			{ -halfSize, halfSize,-halfSize },
			{ -halfSize,-halfSize,-halfSize },
			{ -1,0,0 }
		);

		// Right
		addFace(
			{ halfSize,-halfSize,-halfSize },
			{ halfSize, halfSize,-halfSize },
			{ halfSize, halfSize, halfSize },
			{ halfSize,-halfSize, halfSize },
			{ 1,0,0 }
		);

		// Top
		addFace(
			{ -halfSize, halfSize,-halfSize },
			{ -halfSize, halfSize, halfSize },
			{ halfSize, halfSize, halfSize },
			{ halfSize, halfSize,-halfSize },
			{ 0,1,0 }
		);

		// Bottom
		addFace(
			{ -halfSize,-halfSize, halfSize },
			{ -halfSize,-halfSize,-halfSize },
			{ halfSize,-halfSize,-halfSize },
			{ halfSize,-halfSize, halfSize },
			{ 0,-1,0 }
		);

		AddToGlobalMesh(vertices, indices, offset, ypr);
		UpdateMinMaxBounds();
		UpdateCenterExtend();
	}

	void Mesh::BuildBox(XMFLOAT3 size, XMFLOAT4 color, XMFLOAT3 offset, XMFLOAT3 ypr)
	{
		m_radius = -1;
		m_color = color;
		XMFLOAT3 halfSize = { size.x * 0.5f, size.y * 0.5f, size.z * 0.5f };

		Vector<Vertex> vertices;
		Vector<u32> indices;

		auto addFace = [&](XMFLOAT3 p0, XMFLOAT3 p1, XMFLOAT3 p2, XMFLOAT3 p3, XMFLOAT3 normal)
			{
				u16 start = (u16)vertices.size();

				vertices.push_back({ p0, color, {0.f,1.f}, normal });
				vertices.push_back({ p1, color, {0.f,0.f}, normal });
				vertices.push_back({ p2, color, {1.f,0.f}, normal });
				vertices.push_back({ p3, color, {1.f,1.f}, normal });

				indices.push_back(start + 0);
				indices.push_back(start + 1);
				indices.push_back(start + 2);

				indices.push_back(start + 0);
				indices.push_back(start + 2);
				indices.push_back(start + 3);
			};

		// Front
		addFace(
			{ -halfSize.x,-halfSize.y,-halfSize.z },
			{ -halfSize.x, halfSize.y,-halfSize.z },
			{ halfSize.x, halfSize.y,-halfSize.z },
			{ halfSize.x,-halfSize.y,-halfSize.z },
			{ 0,0,-1 }
		);

		// Back
		addFace(
			{ -halfSize.x,-halfSize.y, halfSize.z },
			{ halfSize.x,-halfSize.y, halfSize.z },
			{ halfSize.x, halfSize.y, halfSize.z },
			{ -halfSize.x, halfSize.y, halfSize.z },
			{ 0,0,1 }
		);

		// Left
		addFace(
			{ -halfSize.x,-halfSize.y, halfSize.z },
			{ -halfSize.x, halfSize.y, halfSize.z },
			{ -halfSize.x, halfSize.y,-halfSize.z },
			{ -halfSize.x,-halfSize.y,-halfSize.z },
			{ -1,0,0 }
		);

		// Right
		addFace(
			{ halfSize.x,-halfSize.y,-halfSize.z },
			{ halfSize.x, halfSize.y,-halfSize.z },
			{ halfSize.x, halfSize.y, halfSize.z },
			{ halfSize.x,-halfSize.y, halfSize.z },
			{ 1,0,0 }
		);

		// Top
		addFace(
			{ -halfSize.x, halfSize.y,-halfSize.z },
			{ -halfSize.x, halfSize.y, halfSize.z },
			{ halfSize.x, halfSize.y, halfSize.z },
			{ halfSize.x, halfSize.y,-halfSize.z },
			{ 0,1,0 }
		);

		// Bottom
		addFace(
			{ -halfSize.x,-halfSize.y, halfSize.z },
			{ -halfSize.x,-halfSize.y,-halfSize.z },
			{ halfSize.x,-halfSize.y,-halfSize.z },
			{ halfSize.x,-halfSize.y, halfSize.z },
			{ 0,-1,0 }
		);

		AddToGlobalMesh(vertices, indices, offset, ypr);
		UpdateMinMaxBounds();
		UpdateCenterExtend();
	}

	void Mesh::BuildCylinder(float radius, int circleStacks, float height, XMFLOAT4 color, XMFLOAT3 offset, XMFLOAT3 ypr)
	{
		if (circleStacks < 3)
			return;

		m_radius = -1;
		m_color = color;

		float halfDepth = height * 0.5f;
		float step = XM_2PI / circleStacks;

		Vector<Vertex> vertices;
		Vector<u32> indices;

		for (int i = 0; i < circleStacks + 1; i++)
		{
			float angle = i * step;

			float x = cosf(angle) * radius;
			float z = sinf(angle) * radius;

			float u = (x / radius) * 0.5f + 0.5f;
			float v = (z / radius) * 0.5f + 0.5f;

			vertices.push_back(Vertex{ XMFLOAT3(x, -halfDepth, z), color, {u, v} });
			vertices.push_back(Vertex{ XMFLOAT3(x,  halfDepth, z), color, {u, v} });
		}

		u16 bottomCenterIndex = (u16)vertices.size();
		vertices.push_back({ XMFLOAT3(0.0f, -halfDepth, 0.0f), color });

		u16 topCenterIndex = (u16)vertices.size();
		vertices.push_back({ XMFLOAT3(0.0f, halfDepth, 0.0f), color });

		for (int i = 0; i < circleStacks; ++i)
		{
			u16 base = i * 2;

			// triangle bottom
			indices.push_back(bottomCenterIndex);
			indices.push_back(base);
			indices.push_back(base + 2);

			// triangle top
			indices.push_back(topCenterIndex);
			indices.push_back(base + 3);
			indices.push_back(base + 1);
		}

		u16 sideStart = (u16)vertices.size();

		for (int i = 0; i <= circleStacks; ++i)
		{
			float u = (float)i / circleStacks;
			float angle = u * XM_2PI;

			float x = cosf(angle) * radius;
			float z = sinf(angle) * radius;

			vertices.push_back({ {x, -halfDepth, z}, color, {u, 1.f} });
			vertices.push_back({ {x,  halfDepth, z}, color, {u, 0.f} });
		}

		for (int i = 0; i < circleStacks; ++i)
		{
			u16 base = sideStart + i * 2;

			indices.push_back(base);
			indices.push_back(base + 1);
			indices.push_back(base + 2);
			indices.push_back(base + 1);
			indices.push_back(base + 3);
			indices.push_back(base + 2);
		}

		AddToGlobalMesh(vertices, indices, offset, ypr);
		UpdateMinMaxBounds();
		UpdateCenterExtend();
		ComputeNormals();
	}

	void Mesh::BuildCone(float baseRadius, int baseStacks, float height, XMFLOAT4 color, XMFLOAT3 offset, XMFLOAT3 ypr)
	{
		if (baseStacks < 3)
			return;

		m_radius = -1;
		m_color = color;

		Vector<Vertex> vertices;
		Vector<u32> indices;

		float halfHeight = height * 0.5f;
		float step = XM_2PI / baseStacks;

		u32 peakIndex = 0;
		vertices.push_back({ {0.f, halfHeight, 0.f}, color, {0.5f, 0.f} });

		u32 sideRingStart = (u32)vertices.size();

		for (int i = 0; i <= baseStacks; ++i)
		{
			float angle = i * step;

			float x = cosf(angle) * baseRadius;
			float z = sinf(angle) * baseRadius;

			float u = (float)i / baseStacks;

			vertices.push_back({ {x, -halfHeight, z}, color, {u, 1.f} });
		}

		for (int i = 0; i < baseStacks; ++i)
		{
			indices.push_back(peakIndex);
			indices.push_back(sideRingStart + i + 1);
			indices.push_back(sideRingStart + i);
		}

		u32 baseRingStart = (u32)vertices.size();

		for (int i = 0; i <= baseStacks; ++i)
		{
			float angle = i * step;

			float x = cosf(angle) * baseRadius;
			float z = sinf(angle) * baseRadius;

			float u = (x / baseRadius) * 0.5f + 0.5f;
			float v = (z / baseRadius) * 0.5f + 0.5f;

			vertices.push_back({ {x, -halfHeight, z}, color, {u, v} });
		}

		u32 baseCenter = (u32)vertices.size();
		vertices.push_back({ {0.f, -halfHeight, 0.f}, color, {0.5f, 0.5f} });

		for (int i = 0; i < baseStacks; ++i)
		{
			indices.push_back(baseCenter);
			indices.push_back(baseRingStart + i);
			indices.push_back(baseRingStart + i + 1);
		}

		AddToGlobalMesh(vertices, indices, offset, ypr);
		UpdateMinMaxBounds();
		UpdateCenterExtend();
		ComputeNormals();
	}

	void Mesh::BuildUvSphere(float radius, int stacks, int slices, XMFLOAT4 color, XMFLOAT3 offset)
	{
		if (stacks < 2)
			return;

		if (slices < 3)
			return;

		m_radius = radius;
		m_color = color;

		Vector<Vertex> vertices;
		Vector<u32> indices;

		for (int y = 0; y <= stacks; ++y) // latitude
		{
			float v = (float)y / stacks;
			float phi = v * XM_PI;

			for (int x = 0; x <= slices; ++x) // longitude
			{
				float u = (float)x / slices;
				float theta = u * XM_2PI;

				XMFLOAT3 point = {};
				point.x = radius * sin(phi) * cos(theta);
				point.y = radius * cos(phi);
				point.z = radius * sin(phi) * sin(theta);

				vertices.push_back(Vertex{ point, color, {u, v} });
			}
		}

		int slicesAmount = slices + 1;

		for (int y = 0; y < stacks; ++y)
		{
			for (int x = 0; x < slices; ++x)
			{
				int nextX = (x + 1);

				u16 a = y * slicesAmount + x;
				u16 b = y * slicesAmount + nextX;
				u16 c = (y + 1) * slicesAmount + x;
				u16 d = (y + 1) * slicesAmount + nextX;

				// triangle 1
				indices.push_back(a);
				indices.push_back(b);
				indices.push_back(c);

				// triangle 2
				indices.push_back(b);
				indices.push_back(d);
				indices.push_back(c);
			}
		}

		AddToGlobalMesh(vertices, indices, offset, {0, 0, 0});
		UpdateMinMaxBounds();
		UpdateCenterExtend();
		ComputeNormals();
	}

	void Mesh::BuildIcoSphere(float radius, int subdivision, DirectX::XMFLOAT4 color, DirectX::XMFLOAT3 offset)
	{
		m_radius = radius;
		m_color = color;

		auto GetMidPoint = [&](u32 i0, u32 i1, Vector<Vertex>& vertices, UnMap<uint64_t, u32>& cache)
			{
				u32 minI = min(i0, i1);
				u32 maxI = max(i0, i1);
				u64 key = ((uint64_t)minI << 32) | maxI;

				auto it = cache.find(key);
				if (it != cache.end()) return it->second;

				XMVECTOR p0 = XMLoadFloat3(&vertices[i0].m_pos);
				XMVECTOR p1 = XMLoadFloat3(&vertices[i1].m_pos);

				XMVECTOR mid = XMVector3Normalize((p0 + p1) * 0.5f) * radius; // projection sur la sphere

				XMFLOAT3 newPos;
				XMStoreFloat3(&newPos, mid);

				Vertex v;
				v.m_pos = newPos;
				v.m_normal = newPos; // normales lissees
				// UV spherique simple
				float u = 0.5f + atan2f(newPos.z, newPos.x) / XM_2PI;
				float vCoord = 0.5f - asinf(newPos.y) / XM_PI;
				v.m_uv = { u, vCoord };
				v.m_color = color;

				vertices.push_back(v);
				u32 index = (u32)vertices.size() - 1;
				cache[key] = index;
				return index;
			};

		Vector<Vertex> vertices;
		Vector<u32> indices;

		const float t = (1.0f + sqrtf(5.0f)) / 2.0f;

		//Icosaedre 12 vertices (Peaks)
		Vector<XMFLOAT3> positions = {
			{-1,  t,  0}, { 1,  t,  0}, {-1, -t,  0}, { 1, -t,  0},
			{ 0, -1,  t}, { 0,  1,  t}, { 0, -1, -t}, { 0,  1, -t},
			{ t,  0, -1}, { t,  0,  1}, {-t,  0, -1}, {-t,  0,  1}
		};

		//Smoothing faces and normals
		for (auto& p : positions)
		{
			XMVECTOR v = XMVector3Normalize(XMLoadFloat3(&p)) * radius;
			XMStoreFloat3(&p, v);

			Vertex vert;
			vert.m_pos = p;
			vert.m_normal = p;
			float u = 0.5f + atan2f(p.z, p.x) / XM_2PI;
			float vCoord = 0.5f - asinf(p.y / radius) / XM_PI;
			vert.m_uv = { u, vCoord };
			vert.m_color = color;
			vertices.push_back(vert);
		}

		// 20 faces of Isocaedre
		u32 faces[60] = {
			0,11,5, 0,5,1, 0,1,7, 0,7,10, 0,10,11,
			1,5,9, 5,11,4, 11,10,2, 10,7,6, 7,1,8,
			3,9,4, 3,4,2, 3,2,6, 3,6,8, 3,8,9,
			4,9,5, 2,4,11, 6,2,10, 8,6,7, 9,8,1
		};

		indices.assign(faces, faces + 60); // indices = faces from 0th to 59th element

		// Subdivisions
		for (int s = 0; s < subdivision; ++s)
		{
			//Prevent to duplicate midpoints due to common neighbors
			UnMap<u64, u32> cache;
			Vector<u32> newIndices;

			for (size_t i = 0; i < indices.size(); i += 3)
			{
				u32 i0 = indices[i];
				u32 i1 = indices[i + 1];
				u32 i2 = indices[i + 2];

				u32 a = GetMidPoint(i0, i1, vertices, cache);
				u32 b = GetMidPoint(i1, i2, vertices, cache);
				u32 c = GetMidPoint(i2, i0, vertices, cache);

				newIndices.insert(newIndices.end(), { i0, a, c, i1, b, a, i2, c, b, a, b, c });
			}

			indices = std::move(newIndices);
		}

		AddToGlobalMesh(vertices, indices, offset, {0, 0, 0});
		UpdateMinMaxBounds();
		UpdateCenterExtend();
		//No compute normals because already done by building it
	}

	void Mesh::BuildCapsule(float radius, int stacks, int slices, float height, DirectX::XMFLOAT4 color, XMFLOAT3 offset, XMFLOAT3 ypr)
	{
		if (stacks < 2)
			return;

		if (slices < 3)
			return;

		m_radius = -1;
		m_color = color;

		Vector<Vertex> vertices;
		Vector<u32> indices;

		float halfHeight = (height - 2.f * radius) * 0.5f;
		float totalHeight = height;

		int slicesAmount = slices + 1; // +1 to close the last vertex

		auto computeV = [&](float y)
			{
				return (y + totalHeight * 0.5f) / totalHeight; // UV
			};

		//Top Half Sphere
		for (int y = 0; y <= stacks; ++y) // latitude
		{
			float v = (float)y / stacks;
			float phi = v * XM_PIDIV2;

			for (int x = 0; x <= slices; ++x) // longitude
			{
				float u = (float)x / slices;
				float theta = u * XM_2PI;

				XMFLOAT3 point = {};
				point.x = radius * sin(phi) * cos(theta);
				point.y = halfHeight + radius * cos(phi);
				point.z = radius * sin(phi) * sin(theta);

				vertices.push_back(Vertex{ point, color, {u, computeV(point.y)} });
			}
		}

		for (int y = 0; y < stacks; ++y)
		{
			for (int x = 0; x < slices; ++x)
			{
				u16 a = y * slicesAmount + x;
				u16 b = a + 1;

				u16 c = (y + 1) * slicesAmount + x;
				u16 d = c + 1;

				// triangle 1
				indices.push_back(a);
				indices.push_back(b);
				indices.push_back(c);

				// triangle 2
				indices.push_back(b);
				indices.push_back(d);
				indices.push_back(c);
			}
		}

		//Bottom Half Sphere
		u16 offsetFromTop = (u16)vertices.size();

		for (int y = 0; y <= stacks; ++y)
		{
			float v = (float)y / stacks;
			float phi = XM_PIDIV2 + v * XM_PIDIV2;

			for (int x = 0; x <= slices; ++x)
			{
				float u = (float)x / slices;
				float theta = u * XM_2PI;

				XMFLOAT3 point = {};
				point.x = radius * sin(phi) * cos(theta);
				point.y = -halfHeight + radius * cos(phi);
				point.z = radius * sin(phi) * sin(theta);

				vertices.push_back(Vertex{ point, color, {u, computeV(point.y)} });
			}
		}
		for (int y = 0; y < stacks; ++y)
		{
			for (int x = 0; x < slices; ++x)
			{
				u16 a = y * slicesAmount + x;
				u16 b = a + 1;

				u16 c = (y + 1) * slicesAmount + x;
				u16 d = c + 1;

				indices.push_back(offsetFromTop + a);
				indices.push_back(offsetFromTop + b);
				indices.push_back(offsetFromTop + c);

				indices.push_back(offsetFromTop + b);
				indices.push_back(offsetFromTop + d);
				indices.push_back(offsetFromTop + c);
			}
		}

		//Quads
		u16 topEquatorStart = stacks * slicesAmount; // last ring of the top sphere where y = stacks
		u16 bottomEquatorStart = offsetFromTop;

		for (int x = 0; x < slices; ++x)
		{
			u16 a = topEquatorStart + x;
			u16 b = topEquatorStart + x + 1;

			u16 c = bottomEquatorStart + x;
			u16 d = bottomEquatorStart + x + 1;

			// triangle 1
			indices.push_back(a);
			indices.push_back(b);
			indices.push_back(c);

			// triangle 2
			indices.push_back(b);
			indices.push_back(d);
			indices.push_back(c);
		}

		AddToGlobalMesh(vertices, indices, offset, ypr);
		UpdateMinMaxBounds();
		UpdateCenterExtend();
		ComputeNormals();
	}

	void Mesh::BuildPlane(DirectX::XMFLOAT2 size, DirectX::XMFLOAT4 color, bool doubleFaced, XMFLOAT3 offset, XMFLOAT3 ypr)
	{
		m_radius = -1;
		m_color = color;

		XMFLOAT2 halfSize = { size.x * 0.5f, size.y * 0.5f };

		Vector<Vertex> vertices;
		Vector<u32> indices;

		vertices =
		{
			{{-halfSize.x, 0, -halfSize.y}, color, {0.f, 1.f}},
			{{-halfSize.x, 0,  halfSize.y}, color, {0.f, 0.f}},
			{{ halfSize.x, 0,  halfSize.y}, color, {1.f, 0.f}},
			{{ halfSize.x, 0, -halfSize.y}, color, {1.f, 1.f}}
		};

		if (!doubleFaced)
		{
			indices =
			{
				0, 1, 2,
				2, 3, 0
			};
		}
		else
		{
			indices =
			{
				0, 1, 2,
				2, 3, 0,

				0, 2, 1,
				2, 0, 3
			};
		}

		AddToGlobalMesh(vertices, indices, offset, ypr);
		UpdateMinMaxBounds();
		UpdateCenterExtend();
		ComputeNormals(false);
	}

	void Mesh::BuildFromVoxels(VoxelGrid& grid, DirectX::XMFLOAT4 color)
	{
		m_radius = -1;
		m_color = color;

		m_vertices.clear();
		m_indices.clear();

		float halfVoxel = 0.5f * grid.voxelSize;

		float gridWidth = grid.width * grid.voxelSize;
		float gridHeight = grid.height * grid.voxelSize;
		float gridDepth = grid.depth * grid.voxelSize;

		XMFLOAT3 gridCenter = { gridWidth * 0.5f - halfVoxel, gridHeight * 0.5f - halfVoxel, gridDepth * 0.5f - halfVoxel };

		for (int z = 0; z < grid.depth; z++)
		{
			for (int y = 0; y < grid.height; y++)
			{
				for (int x = 0; x < grid.width; x++)
				{
					if (!grid.GetAt(x, y, z))
						continue; 

					XMFLOAT3 center(
						x * grid.voxelSize - gridCenter.x,
						y * grid.voxelSize - gridCenter.y,
						z * grid.voxelSize - gridCenter.z
					);

					bool faceLeft = (x == 0) || !grid.GetAt(x - 1, y, z);
					bool faceRight = (x == grid.width - 1) || !grid.GetAt(x + 1, y, z);
					bool faceBottom = (y == 0) || !grid.GetAt(x, y - 1, z);
					bool faceTop = (y == grid.height - 1) || !grid.GetAt(x, y + 1, z);
					bool faceBack = (z == 0) || !grid.GetAt(x, y, z - 1);
					bool faceFront = (z == grid.depth - 1) || !grid.GetAt(x, y, z + 1);

					if (faceLeft)   AddCubeFace(center, halfVoxel, CubeFace::Left, color);
					if (faceRight)  AddCubeFace(center, halfVoxel, CubeFace::Right, color);
					if (faceBottom) AddCubeFace(center, halfVoxel, CubeFace::Bottom, color);
					if (faceTop)    AddCubeFace(center, halfVoxel, CubeFace::Top, color);
					if (faceBack)   AddCubeFace(center, halfVoxel, CubeFace::Back, color);
					if (faceFront)  AddCubeFace(center, halfVoxel, CubeFace::Front, color);
				}
			}
		}

		UpdateMinMaxBounds();
		UpdateCenterExtend();
		ComputeNormals(true);
	}

	void Mesh::AddCubeFace(const XMFLOAT3& center, float halfVoxel, CubeFace face, const XMFLOAT4& color)
	{
		m_radius = -1;

		XMFLOAT3 corners[4];

		switch (face)
		{
		case CubeFace::Front: 
			corners[0] = { center.x - halfVoxel, center.y - halfVoxel, center.z + halfVoxel };
			corners[1] = { center.x - halfVoxel, center.y + halfVoxel, center.z + halfVoxel };
			corners[2] = { center.x + halfVoxel, center.y + halfVoxel, center.z + halfVoxel };
			corners[3] = { center.x + halfVoxel, center.y - halfVoxel, center.z + halfVoxel };
			break;

		case CubeFace::Back: 
			corners[0] = { center.x + halfVoxel, center.y - halfVoxel, center.z - halfVoxel };
			corners[1] = { center.x + halfVoxel, center.y + halfVoxel, center.z - halfVoxel };
			corners[2] = { center.x - halfVoxel, center.y + halfVoxel, center.z - halfVoxel };
			corners[3] = { center.x - halfVoxel, center.y - halfVoxel, center.z - halfVoxel };
			break;

		case CubeFace::Left:
			corners[0] = { center.x - halfVoxel, center.y - halfVoxel, center.z - halfVoxel };
			corners[1] = { center.x - halfVoxel, center.y + halfVoxel, center.z - halfVoxel };
			corners[2] = { center.x - halfVoxel, center.y + halfVoxel, center.z + halfVoxel };
			corners[3] = { center.x - halfVoxel, center.y - halfVoxel, center.z + halfVoxel };
			break;

		case CubeFace::Right:
			corners[0] = { center.x + halfVoxel, center.y - halfVoxel, center.z + halfVoxel };
			corners[1] = { center.x + halfVoxel, center.y + halfVoxel, center.z + halfVoxel };
			corners[2] = { center.x + halfVoxel, center.y + halfVoxel, center.z - halfVoxel };
			corners[3] = { center.x + halfVoxel, center.y - halfVoxel, center.z - halfVoxel };
			break;

		case CubeFace::Top: 
			corners[0] = { center.x - halfVoxel, center.y + halfVoxel, center.z + halfVoxel };
			corners[1] = { center.x - halfVoxel, center.y + halfVoxel, center.z - halfVoxel };
			corners[2] = { center.x + halfVoxel, center.y + halfVoxel, center.z - halfVoxel };
			corners[3] = { center.x + halfVoxel, center.y + halfVoxel, center.z + halfVoxel };
			break;

		case CubeFace::Bottom: 
			corners[0] = { center.x - halfVoxel, center.y - halfVoxel, center.z - halfVoxel };
			corners[1] = { center.x - halfVoxel, center.y - halfVoxel, center.z + halfVoxel };
			corners[2] = { center.x + halfVoxel, center.y - halfVoxel, center.z + halfVoxel };
			corners[3] = { center.x + halfVoxel, center.y - halfVoxel, center.z - halfVoxel };
			break;
		}

		u16 startIndex = (u16)m_vertices.size();

		XMFLOAT2 uv[4] = {
			{0.0f, 1.0f},
			{0.0f, 0.0f},
			{1.0f, 0.0f}, 
			{1.0f, 1.0f}  
		};

		for (int i = 0; i < 4; i++)
		{
			m_vertices.push_back({ corners[i], color, uv[i] });
		}

		m_indices.push_back(startIndex + 0);
		m_indices.push_back(startIndex + 2);
		m_indices.push_back(startIndex + 1);

		m_indices.push_back(startIndex + 0);
		m_indices.push_back(startIndex + 3);
		m_indices.push_back(startIndex + 2);
	}

	void Mesh::MakeVerticesColorFromNormals(DirectX::XMFLOAT4 flatColor, DirectX::XMFLOAT4 slopeColor)
	{
		for (size_t i = 0; i < m_vertices.size(); i++)
		{
			Vertex& vertex = m_vertices[i];

			float y = vertex.m_normal.y;
			float t = abs(y);

			vertex.m_color.x = flatColor.x * t + slopeColor.x * (1.0f - t);
			vertex.m_color.y = flatColor.y * t + slopeColor.y * (1.0f - t);
			vertex.m_color.z = flatColor.z * t + slopeColor.z * (1.0f - t);
			vertex.m_color.w = flatColor.w * t + slopeColor.w * (1.0f - t);
		}
	}

	void Mesh::MakeNoise(int octaves, float frequency, float amplitude)
	{
		PerlinNoise::GeneratePermutations();

		UnMap<size_t, XMFLOAT3> displacementMap;

		for (size i = 0; i < m_vertices.size(); i++)
		{
			Vertex& vertex = m_vertices[i];
			size hash = PointHasher::Hash(vertex.m_pos);

			if (displacementMap.find(hash) == displacementMap.end())
			{
				float noise = 0.f;
				float tempFrequency = frequency;
				float tempAmplitude = amplitude;

				for (int octave = 0; octave < octaves; ++octave)
				{
					XMFLOAT3 fPos;
					XMStoreFloat3(
						&fPos,
						XMVectorScale(
							XMLoadFloat3(&vertex.m_pos),
							tempFrequency
						)
					);

					noise += PerlinNoise::Make(fPos) * tempAmplitude;
					tempFrequency *= 2.f;
					tempAmplitude *= 0.5f;
				}

				XMFLOAT3 displacement = { noise, noise, noise };
				displacementMap[hash] = displacement;
			}
		}

		for (size i = 0; i < m_vertices.size(); i++)
		{
			Vertex& vertex = m_vertices[i];
			size hash = PointHasher::Hash(vertex.m_pos);

			XMStoreFloat3(
				&vertex.m_pos,
				XMVectorAdd(
					XMLoadFloat3(&vertex.m_pos),
					XMLoadFloat3(&displacementMap[hash])
				)
			);
			XMFLOAT3 a = displacementMap[hash];
			int b = 0;
		}

		UpdateMinMaxBounds();
		UpdateCenterExtend();
		ComputeNormals(true);
	}

	void Mesh::AddTriangle(XMFLOAT3 p1, XMFLOAT3 p2, XMFLOAT3 p3, XMFLOAT4 color, bool doubleFaced)
	{
		m_radius = -1;

		m_useTexture = false;

		u16 offset = (u16)m_vertices.size();

		m_vertices.push_back(Vertex{ p1, color });
		m_vertices.push_back(Vertex{ p2, color }),
		m_vertices.push_back(Vertex{ p3, color });

		m_indices.push_back(offset + 0);
		m_indices.push_back(offset + 1);
		m_indices.push_back(offset + 2);

		if (doubleFaced)
		{
			m_indices.push_back(offset + 0);
			m_indices.push_back(offset + 2);
			m_indices.push_back(offset + 1);
		}

		ComputeNormals();
	}

	void Mesh::LoadObj(const std::wstring path, XMFLOAT3 color)
	{
		m_radius = -1;
		m_vertices.clear();
		m_indices.clear();
		m_color = { color.x, color.y, color.z, 1 };

		if (ObjParser::LoadObj(this, path) == false)
		{
			_ASSERT(false && "Obj couldn't be read !");
			return;
		}

		UpdateMinMaxBounds();
		UpdateCenterExtend();
	}

	void Mesh::LoadGlb(const std::wstring path)
	{
		m_radius = -1;
		m_vertices.clear();
		m_indices.clear();

		if (GlbParser::LoadGlb(this, path) == false)
		{
			_ASSERT(false && "Glb couldn't be read !");
			return;
		}

		UpdateMinMaxBounds();
		UpdateCenterExtend();
	}

	void Mesh::SetMaterialTag(int tag)
	{
		m_materialTag = tag;
	}

	const int Mesh::GetMaterialTag() const
	{
		return m_materialTag;
	}

	void Mesh::SetAnimData(MeshAnimationModel* m)
	{
		mp_animModel = m;
	}

	void Mesh::UpdateMinMaxBounds()
	{
		if (m_vertices.size() == 0)
			return;
		XMFLOAT3 pos0 = m_vertices[0].m_pos;
		m_maxBounds.x = pos0.x;
		m_maxBounds.y = pos0.y;
		m_maxBounds.z = pos0.z;
		m_minBounds.x = pos0.x;
		m_minBounds.y = pos0.y;
		m_minBounds.z = pos0.z;

		for (size_t i = 1; i < m_vertices.size(); i++)
		{
			XMFLOAT3 pos = m_vertices[i].m_pos;

			if (pos.x > m_maxBounds.x)
				m_maxBounds.x = pos.x;

			if (pos.y > m_maxBounds.y)
				m_maxBounds.y = pos.y;

			if (pos.z > m_maxBounds.z)
				m_maxBounds.z = pos.z;

			if (pos.x < m_minBounds.x)
				m_minBounds.x = pos.x;

			if (pos.y < m_minBounds.y)
				m_minBounds.y = pos.y;

			if (pos.z < m_minBounds.z)
				m_minBounds.z = pos.z;
		}
	}

	void Mesh::CopyDataTo(Mesh* p_target)
	{
		p_target->m_vertices = m_vertices;
		p_target->m_indices = m_indices;
		p_target->m_color = m_color;
		p_target->m_useTexture = m_useTexture;
		p_target->m_textureTag = m_textureTag;

		p_target->UpdateMinMaxBounds();
		p_target->UpdateCenterExtend();
	}

	void Mesh::CopyDataFrom(Mesh* p_source)
	{
		m_vertices = p_source->m_vertices;
		m_indices = p_source->m_indices;
		m_color = p_source->m_color;
		m_useTexture = p_source->m_useTexture;
		m_textureTag = p_source->m_textureTag;

		UpdateMinMaxBounds();
		UpdateCenterExtend();
	}

	void Mesh::UpdateCenterExtend()
	{
		m_center.x = (m_minBounds.x + m_maxBounds.x) * 0.5f;
		m_center.y = (m_minBounds.y + m_maxBounds.y) * 0.5f;
		m_center.z = (m_minBounds.z + m_maxBounds.z) * 0.5f;
		m_extend.x = m_maxBounds.x - m_center.x;
		m_extend.y = m_maxBounds.y - m_center.y;
		m_extend.z = m_maxBounds.z - m_center.z;
	}

	XMVECTOR Mesh::ComputeNormal(FXMVECTOR p0, FXMVECTOR p1, FXMVECTOR p2)
	{
		XMVECTOR u = p1 - p0;
		XMVECTOR v = p2 - p0;
		return XMVector3Normalize(
			XMVector3Cross(u, v));
	}

	float Mesh::GetBoundingSphereRadius()
	{
		if (m_radius != -1)
			return m_radius;

		XMVECTOR vMin = XMLoadFloat3(&m_minBounds);
		XMVECTOR vMax = XMLoadFloat3(&m_maxBounds);
		XMVECTOR vCenter = XMLoadFloat3(&m_center);

		XMVECTOR vRadiusVec = XMVectorSubtract(vMax, vCenter);
		float radius = XMVectorGetX(XMVector3Length(vRadiusVec));

		//Margin
		return radius * 1.5f;
	}
}