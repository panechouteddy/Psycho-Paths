#pragma once
#include "GameObject.h"
#include "CaveSections.h"
#include "CaveGeneratorParams.h"
#include <optional>

namespace nam
{
    class CaveGenerator
    {
    private:
        Scene* mp_scene;
        CaveSections* mp_caveSections;
        Vector<DirectX::XMFLOAT3> m_tunnelEnds;

    public:
        CaveGenerator(Scene* scene, const CaveSectionParams& sectionParams, u8 meshCount = 12);
        ~CaveGenerator();

        void Generate(const CaveGeneratorParams& params, const DirectX::XMFLOAT3& start, std::optional<DirectX::XMFLOAT3> end = std::nullopt);
        void PopGeneration();
        Vector<CaveTubeSection>* GetLastTubeSections();

    private:
        void GenerateTunnel(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, float radius);

    };
}