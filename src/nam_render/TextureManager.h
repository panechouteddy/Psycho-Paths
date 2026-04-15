#pragma once
#define PATH_TEXTURES_FOLDER L"../../res/Textures/"
#include "Constants.h"

namespace nam
{
	struct Texture
	{
		bool m_isLoaded = false;

		size m_heapOffset = -1;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_texture = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_textureUploadHeap = nullptr;

	public:
		~Texture() = default;
		const size& GetRootIndex() const;
		ID3D12Resource* Resource();

	private:
		void Load(std::wstring path);

		friend class TextureManager;
	};

	class TextureManager
	{
		UnMap<size, Texture*> m_textures;
	public:
		~TextureManager();

		void UnInit();

		// if True, you just type the name without the path
		void CreateTexture(std::wstring path, size uniqueTag, bool usingTextureFolder = false);

		Texture* GetTexture(size uniqueTag) const;
	};
}

