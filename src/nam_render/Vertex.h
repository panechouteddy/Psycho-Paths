#pragma once
#include <DirectXMath.h>

namespace nam
{
    struct Vertex
    {
        DirectX::XMFLOAT3 m_pos = { 0, 0, 0 };
        DirectX::XMFLOAT4 m_color = {1, 1, 1, 1};
        DirectX::XMFLOAT2 m_uv = { 0, 0 };
        DirectX::XMFLOAT3 m_normal = { 0, 0, 0 };

        DirectX::XMINT4 m_bonesIndices = { -1, -1, -1, -1 };
        DirectX::XMFLOAT4 m_bonesWeights = {0, 0, 0 ,0};
    };
}
