#include "pch.h"
#include "ARenderItem.h"
#include "RenderManager.h"
#include "CommandManager.h"

namespace nam
{
	ARenderItem::~ARenderItem()
	{
	}

	void ARenderItem::SetToDynamic(u32 maxVertices, u32 maxIndices)
	{
		if (m_bufferInit)
		{
			_ASSERT(false && "Buffer already init as Default, you need to call it before Run() if you want a Dynamic");
			return;
		}

		m_max_vertices = maxVertices;
		m_max_indices = maxIndices;
		m_isDefaultBuffer = false;
	}

	void ARenderItem::InitEmptyBuffers()
	{
		if (m_bufferInit)
			return;

		if (m_isDefaultBuffer)
			InitAsDefaultHeaps();
		else
			InitAsUploadHeaps();

		m_bufferInit = true;
	}

	void ARenderItem::UpdateBuffers()
	{
		if (m_isDefaultBuffer)
			return;

		if (m_dirty == false) //If no changes
			return;

		m_vbByteSize = (UINT)m_vertices.size() * sizeof(Vertex);
		m_ibByteSize = (UINT)m_indices.size() * sizeof(u32);

		if (m_vertices.size() > m_max_vertices)
		{
			_ASSERT(false && "Error : Too much Vertices");
			return;
		}

		if (m_indices.size() > m_max_indices)
		{
			_ASSERT(false && "Error : Too much Indices MAX");
			return;
		}

		ID3D12GraphicsCommandList* p_commandList = RENDER_MANAGER->GetCommandManager()->GetCommandList();

		memcpy(m_mappedVertices, m_vertices.data(), m_vbByteSize);
		memcpy(m_mappedIndices, m_indices.data(), m_ibByteSize);

		m_dirty = false;
	}

	void ARenderItem::BindBuffers()
	{
		ID3D12GraphicsCommandList* p_commandList = RENDER_MANAGER->GetCommandManager()->GetCommandList();
		D3D12_VERTEX_BUFFER_VIEW vbv = {};
		D3D12_INDEX_BUFFER_VIEW ibv = {};

		if (m_isDefaultBuffer)
		{
			vbv = VertexBufferView();
			ibv = IndexBufferView();
		}
		else
		{
			vbv = VertexBufferUploaderView();
			ibv = IndexBufferUploaderView();
		}

		p_commandList->IASetVertexBuffers(0, 1, &vbv);
		p_commandList->IASetIndexBuffer(&ibv);
	}

	void ARenderItem::SetColor(XMFLOAT4 color)
	{
		m_color = color;

		for (Vertex& v : m_vertices)
		{
			v.m_color = color;
		}

		m_dirty = true;
	}
	void ARenderItem::SetTexture(int textureTag)
	{
		if (textureTag < 0)
			m_useTexture = false;
		else
			m_useTexture = true;

		m_textureTag = textureTag;
	}
	const int ARenderItem::IsUsingTexture() const
	{
		return (m_useTexture == true) ? 1 : 0;
	}
	const UINT ARenderItem::GetTextureTag() const
	{
		return static_cast<UINT>(m_textureTag);
	}

	void ARenderItem::MakeRainbowVertices()
	{
		auto HSVtoRGB = [](float h, float s, float v) -> XMFLOAT3
			{
				float c = v * s;
				float hPrime = h / 60.0f;
				float x = c * (1.0f - fabsf(fmodf(hPrime, 2.0f) - 1.0f));

				XMFLOAT3 rgb;

				if (hPrime < 1)      rgb = { c, x, 0 };
				else if (hPrime < 2) rgb = { x, c, 0 };
				else if (hPrime < 3) rgb = { 0, c, x };
				else if (hPrime < 4) rgb = { 0, x, c };
				else if (hPrime < 5) rgb = { x, 0, c };
				else                 rgb = { c, 0, x };

				float m = v - c;
				rgb.x += m;
				rgb.y += m;
				rgb.z += m;

				return rgb;
			};

		int size = (int)m_vertices.size();

		for (size_t i = 0; i < size; i++)
		{
			float t = (float)(i) / (float)(size);

			float hue = t * 360.f;

			XMFLOAT3 color = HSVtoRGB(hue, 1.f, 1.f);

			m_vertices[i].m_color = XMFLOAT4(color.x, color.y, color.z, 1.f);
		}
	}

	void ARenderItem::SetManualUVs(const Vector<DirectX::XMFLOAT2>& uvs)
	{
		if (uvs.size() != m_vertices.size())
		{
			_ASSERT(false && "UV amount != Vertices amount");
			return;
		}

		for (size_t i = 0; i < m_vertices.size(); ++i)
		{
			m_vertices[i].m_uv = uvs[i];
		}
	}

	D3D12_VERTEX_BUFFER_VIEW ARenderItem::VertexBufferView() const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = mp_vertexBufferGPU->GetGPUVirtualAddress();
		vbv.StrideInBytes = m_vertexByteStride;
		vbv.SizeInBytes = m_vbByteSize;

		return vbv;
	}
	D3D12_INDEX_BUFFER_VIEW ARenderItem::IndexBufferView() const
	{
		D3D12_INDEX_BUFFER_VIEW ibv;
		ibv.BufferLocation = mp_indexBufferGPU->GetGPUVirtualAddress();
		ibv.Format = m_indexFormat;
		ibv.SizeInBytes = m_ibByteSize;

		return ibv;
	}

	D3D12_VERTEX_BUFFER_VIEW ARenderItem::VertexBufferUploaderView() const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = mp_vertexBufferUploader->GetGPUVirtualAddress();
		vbv.StrideInBytes = m_vertexByteStride;
		vbv.SizeInBytes = m_vbByteSize;

		return vbv;
	}

	D3D12_INDEX_BUFFER_VIEW ARenderItem::IndexBufferUploaderView() const
	{
		D3D12_INDEX_BUFFER_VIEW ibv;
		ibv.BufferLocation = mp_indexBufferUploader->GetGPUVirtualAddress();
		ibv.Format = m_indexFormat;
		ibv.SizeInBytes = m_ibByteSize;

		return ibv;
	}

	const PsoType ARenderItem::GetPsoToUse() const
	{
		return m_psoToUse;
	}

	const u32& ARenderItem::GetID() const
	{
		return m_id;
	}

	const bool ARenderItem::IsVisible() const
	{
		return m_visibility;
	}

	void ARenderItem::SetVisibilty(bool state)
	{
		m_visibility = state;
	}

	MeshAnimationModel* ARenderItem::GetAnimModel() const
	{
		return mp_animModel;
	}

	void ARenderItem::AddToGlobalMesh(Vector<Vertex>& vertices, Vector<u32>& indices, DirectX::XMFLOAT3 offset, DirectX::XMFLOAT3 ypr)
	{
		m_dirty = true;

		XMMATRIX transform =
			XMMatrixRotationRollPitchYaw(ypr.x, ypr.y, ypr.z) *
			XMMatrixTranslation(offset.x, offset.y, offset.z);

		u32 indiceOffset = static_cast<u32>(m_vertices.size());

		for (u32& i : indices)
		{
			m_indices.push_back(indiceOffset + i);
		}

		for (Vertex& v : vertices)
		{
			XMVECTOR p = XMLoadFloat3(&v.m_pos);
			p = XMVector3Transform(p, transform);
			XMStoreFloat3(&v.m_pos, p);

			m_vertices.push_back(v);
		}
	}

	void ARenderItem::InitAsDefaultHeaps()
	{
		m_vertexByteStride = sizeof(Vertex);

		m_vbByteSize = (UINT)m_vertices.size() * sizeof(Vertex);
		m_ibByteSize = (UINT)m_indices.size() * sizeof(u32);

		if (mp_vertexBufferCPU == nullptr)
		{
			ThrowIfFailed(D3DCreateBlob(m_vbByteSize, &mp_vertexBufferCPU));
			CopyMemory(mp_vertexBufferCPU->GetBufferPointer(), m_vertices.data(), m_vbByteSize);
		}

		if (mp_indexBufferCPU == nullptr)
		{
			ThrowIfFailed(D3DCreateBlob(m_ibByteSize, &mp_indexBufferCPU));
			CopyMemory(mp_indexBufferCPU->GetBufferPointer(), m_indices.data(), m_ibByteSize);
		}

		ID3D12Device* p_device = RENDER_MANAGER->GetDevice();
		ID3D12GraphicsCommandList* p_commandList = RENDER_MANAGER->GetCommandManager()->GetCommandList();

		if (mp_vertexBufferGPU == nullptr)
		{
			mp_vertexBufferGPU = d3dUtil::CreateDefaultBuffer(p_device,
				p_commandList, m_vertices.data(), m_vbByteSize, mp_vertexBufferUploader);

			mp_vertexBufferGPU->SetName(L"Vertex Buffer GPU Resource");
			mp_vertexBufferUploader->SetName(L"Vertex Buffer Uploader Resource");
		}

		if (mp_indexBufferGPU == nullptr)
		{
			mp_indexBufferGPU = d3dUtil::CreateDefaultBuffer(p_device,
				p_commandList, m_indices.data(), m_ibByteSize, mp_indexBufferUploader);

			mp_indexBufferGPU->SetName(L"Index Buffer GPU Resource");
			mp_indexBufferUploader->SetName(L"Index Buffer Uploader Resource");
		}
	}

	void ARenderItem::InitAsUploadHeaps()
	{
		if (m_max_vertices <= 0 || m_max_indices <= 0)
		{
			_ASSERT(false && "ARenderItem : SET THE MAX_VERTICES AND MAX_INDICES TO INIT A UPLOADHEAP");
			return;
		}

		m_vertexByteStride = sizeof(Vertex);

		m_vbByteSize = (UINT)m_vertices.size() * sizeof(Vertex);
		m_ibByteSize = (UINT)m_indices.size() * sizeof(u32);

		ID3D12Device* p_device = RENDER_MANAGER->GetDevice();

		// Vertex buffer
		D3D12_HEAP_PROPERTIES heapProps = {};
		heapProps.Type = D3D12_HEAP_TYPE_UPLOAD; // mappable
		heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

		D3D12_RESOURCE_DESC vbDesc = CD3DX12_RESOURCE_DESC::Buffer(m_max_vertices * sizeof(Vertex));
		p_device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&vbDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ, // prêt pour lecture GPU
			nullptr,
			IID_PPV_ARGS(&mp_vertexBufferUploader)
		);

		// Map permanent
		mp_vertexBufferUploader->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedVertices));

		// Index buffer
		D3D12_RESOURCE_DESC ibDesc = CD3DX12_RESOURCE_DESC::Buffer(m_max_indices * sizeof(u32));
		p_device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&ibDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&mp_indexBufferUploader)
		);

		mp_indexBufferUploader->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedIndices));
	}
}


