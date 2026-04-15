#pragma once
#include "DirectXMath.h"
#include <PlaceableElement.h>

namespace nam
{
    struct CaveSectionParams
    {
        float m_width = 6.f;
        float m_height = 6.f;
        float m_depth = 6.f;
        float m_voxelSize = 2.f;
        float m_noiseFrequency = 1.5f;
        float m_noiseAmplitude = 2.f;
        int m_noiseOctaves = 4;
        size m_textureTag = 0;
        DirectX::XMFLOAT4 m_color = { 0.7f, 0.7f, 0.7f, 1.f };

        Vector<PlaceableElement> m_placeableElements;
        float m_spawnDensity = 1.f;
    };
}