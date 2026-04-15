#pragma once
#include "GameObject.h"
#include "Mesh.h"
#include "CaveSectionParams.h"
#include "CaveTubeSection.h"

namespace nam
{
    class CaveSections
    {
    private:
        Scene* mp_scene;
        Vector<Mesh*> m_wallMeshes;
        Mesh* m_floorMesh;
        CaveSectionParams m_params;

        Queue<Vector<CaveTubeSection>> m_tubeSections;
    public:
        CaveSections(Scene* scene, const CaveSectionParams& params);
        ~CaveSections();

        void BeginGeneration();
        void PopGeneration();
        Vector<CaveTubeSection>* GetLastCaveTubeSections();

        void GenerateMeshes(u8 count);

        void CreateTube(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, float radius);

    private:
        void CreateWall(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& baseRotation, const DirectX::XMFLOAT3& scale, const DirectX::XMFLOAT3& normal, DirectX::XMFLOAT3 worldRotation, CaveTubeSection& caveTubeSection, bool fromFloor);
        const PlaceableElement* ChooseRandomElement(const Vector<PlaceableElement>& elements);
        void GenerateWallMeshes(u8 count);
        void GenerateFloorMesh();
    };
}