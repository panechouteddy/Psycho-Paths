#pragma once
#include "ContainerSequential.h"

namespace nam
{
    struct InputLayoutElementDesc
    {
        std::string semanticName;   // "POSITION", "COLOR", "TEXCOORD"
        UINT semanticIndex = 0;
        DXGI_FORMAT format;         // DXGI_FORMAT_R32G32B32_FLOAT
        UINT inputSlot = 0;
        UINT alignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
        D3D12_INPUT_CLASSIFICATION inputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        UINT instanceDataStepRate = 0;
    };

    class InputLayout
    {
        bool m_dirty = true;

        Vector<InputLayoutElementDesc> m_elements;
        Vector<D3D12_INPUT_ELEMENT_DESC> m_rawElements;
        mutable D3D12_INPUT_LAYOUT_DESC m_d3d12Desc = {};

    public:
        void Add(const std::string& semantic, UINT semanticIndex = 0, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN, UINT inputSlot = 0, UINT alignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT);
        void Build(); // To use when all Add done

        D3D12_INPUT_LAYOUT_DESC GetDesc() const;
    };
}
