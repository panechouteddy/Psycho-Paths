#pragma once
#include "CaveSectionParams.h"

namespace nam
{
    struct CaveGeneratorParams
    {
        int m_seed = 0;
        float m_tunnelRadius = 20.f;
        float m_tunnelLength = 50.f;
        CaveSectionParams m_sectionParams;
    };
}