#include "pch.h"
#include "TextureManager.h"
#include "RenderManager.h"
#include "CommandManager.h"

namespace nam
{
	const size& Texture::GetRootIndex() const
	{
		return m_heapOffset;
	}
	ID3D12Resource* Texture::Resource()
	{
		return m_texture.Get();
	}
	void Texture::Load(std::wstring path)
	{
		if (m_isLoaded)
		{
			_ASSERT(false && "Texture already Loaded !");
			return;
		}

		std::wstring file = path;

		RenderManager* p_renderManager = RENDER_MANAGER;
		ID3D12Device* p_device = p_renderManager->GetDevice();

		ThrowIfFailed(
			CreateDDSTextureFromFile12(
				p_device,
				p_renderManager->GetCommandManager()->GetCommandList(),
				file.c_str(),
				m_texture,
				m_textureUploadHeap
			)
		);

		CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(
			p_renderManager->GetDescriptorHeapsWrapper()->GetGlobal_Heap()->GetCPUDescriptorHandleForHeapStart(),
			(int)m_heapOffset,
			p_renderManager->GetDescriptorHeapsWrapper()->m_cbvSrvUavDescriptorSize
		);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = m_texture->GetDesc().Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = m_texture->GetDesc().MipLevels;

		p_device->CreateShaderResourceView(
			m_texture.Get(),
			&srvDesc,
			cpuHandle
		);

		m_texture->SetName(L"Texture Resource");
		m_textureUploadHeap->SetName(L"Texture UploadHeap Resource");

		m_isLoaded = true;
	}

	TextureManager::~TextureManager()
	{
		UnInit();
	}

	void TextureManager::UnInit()
	{
		for (auto& [tag, texture] : m_textures)
		{
			delete texture;
		}

		m_textures.clear();
	}

	void TextureManager::CreateTexture(std::wstring path, size uniqueTag, bool usingTextureFolder)
	{
		auto it = m_textures.find(uniqueTag);

		if (it != m_textures.end())
			return;

		Texture* p_texture = new Texture();
		p_texture->m_heapOffset = uniqueTag;

		std::wstring fullPath = L"";

		if (usingTextureFolder)
			fullPath = PATH_TEXTURES_FOLDER + path;
		else
			fullPath = path;

		p_texture->Load(fullPath);

		m_textures[uniqueTag] = p_texture;
	}

	Texture* TextureManager::GetTexture(size uniqueTag) const
	{
		auto it = m_textures.find(uniqueTag);

		if (it == m_textures.end())
			return nullptr;

		return it->second;
	}
}
