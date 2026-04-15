#pragma once
#include "Vertex.h"
#include "Common/d3dUtil.h"
#include "PipelineStateBuilder.h"
#include "Constants.h"

#include <BaseTypes.h>

namespace nam
{
    constexpr u32 INVALID_ID = u32(-1);

	enum class RenderItemType
	{
		_None = -1,
		_Mesh,
		_Sprite,
		_Text
	};

    struct MeshAnimationModel;

	struct ARenderItem
	{
    public:
		RenderItemType m_type = RenderItemType::_None;
        PsoType m_psoToUse = PsoType::_Undefined;

        DirectX::XMFLOAT4 m_color = {};

        bool m_useTexture = false;
        size m_textureTag = -1; // setted with SetTexture()

        Vector<Vertex> m_vertices = {};
        Vector<u32> m_indices = {};

        UINT m_vertexByteStride = 0;
        DXGI_FORMAT m_indexFormat = DXGI_FORMAT_R32_UINT;

        UINT m_vbByteSize = {};
        UINT m_ibByteSize = {};

        //Blobs
        Microsoft::WRL::ComPtr<ID3DBlob> mp_vertexBufferCPU = nullptr;
        Microsoft::WRL::ComPtr<ID3DBlob> mp_indexBufferCPU = nullptr;

        //Default Heap
        Microsoft::WRL::ComPtr<ID3D12Resource> mp_vertexBufferGPU = nullptr;
        Microsoft::WRL::ComPtr<ID3D12Resource> mp_indexBufferGPU = nullptr;

        //Upload Heap
        Microsoft::WRL::ComPtr<ID3D12Resource> mp_vertexBufferUploader = nullptr;
        Microsoft::WRL::ComPtr<ID3D12Resource> mp_indexBufferUploader = nullptr;

    protected:
        bool m_bufferInit = false;

        bool m_isDefaultBuffer = true; // If false, the buffer can be updated after the init
        bool m_dirty = true; // If not DefaultBuffer, when dirty, the buffer will be updated 

        u32 m_max_vertices = 0;
        u32 m_max_indices = 0;

        void* m_mappedVertices = nullptr;
        void* m_mappedIndices = nullptr;

        u32 m_id = INVALID_ID;

        bool m_visibility = true;

        MeshAnimationModel* mp_animModel = nullptr;
    public:
        ~ARenderItem();

        //Allow to changes the datas of the RenderItem dynamically, Call it before the Run()
        //If not called, it will be init as a Static (Default Heap Buffer)
        void SetToDynamic(u32 maxVertices, u32 maxIndices);

        void InitEmptyBuffers();
        void UpdateBuffers(); // Useful for the Upload Heaps Only
        void BindBuffers();

        void SetColor(DirectX::XMFLOAT4 color);
        void SetTexture(int textureTag);
        const int IsUsingTexture() const;
        const UINT GetTextureTag() const;

        void MakeRainbowVertices();
        void SetManualUVs(const Vector<DirectX::XMFLOAT2>& uvs);

        D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const;
        D3D12_INDEX_BUFFER_VIEW IndexBufferView() const;

        D3D12_VERTEX_BUFFER_VIEW VertexBufferUploaderView() const;
        D3D12_INDEX_BUFFER_VIEW IndexBufferUploaderView() const;

        const PsoType GetPsoToUse() const;
        const u32& GetID() const;

        virtual const int GetMaterialTag() const { return MAX_MATERIALS - 1; };

        const bool IsVisible() const;
        void SetVisibilty(bool state);

        MeshAnimationModel* GetAnimModel() const;

    protected:
        void AddToGlobalMesh(Vector<Vertex>& vertices, Vector<u32>& indices, DirectX::XMFLOAT3 offset, DirectX::XMFLOAT3 ypr);

    private:
        void InitAsDefaultHeaps();
        void InitAsUploadHeaps(); // Depends of the struct

        friend class RenderItemManager;
	};
}

