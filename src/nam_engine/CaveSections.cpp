#include "pch.h"
#include "CaveSections.h"
#include "MeshRendererComponent.h"
#include "PerlinNoise.h"
#include "RenderManager.h"
#include "Rng.h"
#include <VoxelGrid.h>
#include "PoissonDisc.h"
#include "BoxColliderComponent.h"

using namespace DirectX;

namespace nam
{
    CaveSections::CaveSections(Scene* scene, const CaveSectionParams& params)
        : mp_scene(scene), m_params(params), m_floorMesh(nullptr)
    {
    }

    CaveSections::~CaveSections()
    {
    }

    void CaveSections::BeginGeneration()
    {
        m_tubeSections.push({});
    }

    void CaveSections::PopGeneration()
    {
        if (m_tubeSections.empty())
            return;

        Vector<CaveTubeSection>& caveTubeSections = m_tubeSections.front();
        for (size_t i = 0; i < caveTubeSections.size(); i++)
        {
            CaveTubeSection& caveTubeSection = caveTubeSections[i];
            for (size_t j = 0; j < caveTubeSection.m_gameObjects.size(); j++)
            {
                GameObject* go = caveTubeSection.m_gameObjects[j];
                mp_scene->DestroyGameObject(go);
            }
        }

        m_tubeSections.pop();
    }

    Vector<CaveTubeSection>* CaveSections::GetLastCaveTubeSections()
    {
        if (m_tubeSections.empty())
            return nullptr;
        return &m_tubeSections.back();
    }

    void CaveSections::GenerateMeshes(u8 count)
    {
        GenerateWallMeshes(count);
        GenerateFloorMesh();
    }

    void CaveSections::CreateTube(const XMFLOAT3& start, const XMFLOAT3& end, float radius)
    {
        if (m_tubeSections.empty())
            return;

        CaveTubeSection caveTubeSection;
        caveTubeSection.m_start = start;
        caveTubeSection.m_end = end;
        caveTubeSection.m_radius = radius;

        XMVECTOR vStart = XMLoadFloat3(&start);
        XMVECTOR vEnd = XMLoadFloat3(&end);

        XMVECTOR vDirection = XMVectorSubtract(vEnd, vStart);
        float length;
        XMStoreFloat(&length, XMVector3Length(vDirection));
        vDirection = XMVector3Normalize(vDirection);

        XMVECTOR vMiddle = XMVectorScale(XMVectorAdd(vStart, vEnd), 0.5f);
        XMFLOAT3 middle;
        XMStoreFloat3(&middle, vMiddle);

        XMVECTOR vWorldUp = XMVectorSet(0, 1, 0, 0);

        XMVECTOR vDot = XMVector3Dot(vDirection, vWorldUp);
        if (fabs(XMVectorGetX(vDot)) > 0.99f)
            vWorldUp = XMVectorSet(0, 0, 1, 0);

        XMVECTOR vRight = XMVector3Normalize(XMVector3Cross(vWorldUp, vDirection));
        XMVECTOR vUp = XMVector3Normalize(XMVector3Cross(vDirection, vRight));

        float worldYaw = atan2f(XMVectorGetX(vDirection), XMVectorGetZ(vDirection));
        float worldPitch = asinf(-XMVectorGetY(vDirection));
        float worldRoll = 0.0f;

        XMFLOAT3 wallSize = { m_params.m_width, m_params.m_height, m_params.m_depth };

        Vector<XMFLOAT3> baseRotation =
        {
            { 0, XM_PIDIV2, 0 },
            { XM_PIDIV2, 0, XM_PIDIV2 },
            { -XM_PIDIV2, 0, XM_PIDIV2 },
            { 0, -XM_PIDIV2, 0 }
        };

        Vector<XMFLOAT3> basePosOffset =
        {
            { 0, -radius, 0 },
            { -radius, 0, 0 },
            { radius, 0, 0 },
            { 0, radius, 0 },
        };

        float scaleX = (radius * 2.f) / wallSize.x;
        float scaleY = (length * 1.25f) / wallSize.y;

        for (size_t i = 0; i < baseRotation.size(); i++)
        {
            XMFLOAT3 pos;
            pos.x = middle.x + basePosOffset[i].x * XMVectorGetX(vRight) + basePosOffset[i].y * XMVectorGetX(vUp) + basePosOffset[i].z * XMVectorGetX(vDirection);
            pos.y = middle.y + basePosOffset[i].x * XMVectorGetY(vRight) + basePosOffset[i].y * XMVectorGetY(vUp) + basePosOffset[i].z * XMVectorGetY(vDirection);
            pos.z = middle.z + basePosOffset[i].x * XMVectorGetZ(vRight) + basePosOffset[i].y * XMVectorGetZ(vUp) + basePosOffset[i].z * XMVectorGetZ(vDirection);

            XMVECTOR vNormalDir = XMVectorSubtract(vMiddle, XMLoadFloat3(&pos));
            vNormalDir = XMVector3Normalize(vNormalDir);

            XMFLOAT3 normal;
            XMStoreFloat3(&normal, vNormalDir);

            CreateWall(pos, baseRotation[i], { scaleX, scaleY,1 }, normal, { worldYaw, worldPitch, worldRoll }, caveTubeSection, i == 0);
        }

        m_tubeSections.back().push_back(caveTubeSection);
    }

    void CaveSections::CreateWall(const XMFLOAT3& position, const XMFLOAT3& baseRotation, const XMFLOAT3& scale, const XMFLOAT3& normal, XMFLOAT3 worldRotation, CaveTubeSection& caveTubeSection, bool fromFloor)
    {
        if (m_wallMeshes.size() == 0)
            return;

        GameObject* wall = mp_scene->CreateGameObject<GameObject>();
        caveTubeSection.m_gameObjects.push_back(wall);
        MeshRendererComponent renderer;
        BoxColliderComponent collider;
        wall->AddComponent<BoxColliderComponent>(collider);
        wall->SetActive(true);

        if (fromFloor)
        {
            renderer.SetMeshInstance(m_floorMesh);
        }
        else
        {
            int meshIndex = Rng::Int(0, (int)m_wallMeshes.size() - 1);
            renderer.SetMeshInstance(m_wallMeshes[meshIndex]);
        }

        wall->AddComponent(renderer);

        TransformComponent* p_transform = wall->mp_transform;
        p_transform->SetWorldPosition(position);
        p_transform->SetWorldYPR(baseRotation.x, baseRotation.y, baseRotation.z);
        p_transform->RotateWorldYPR(worldRotation.x, worldRotation.y, worldRotation.z);
        p_transform->SetWorldScale(scale);

        if (fromFloor)
            return;

        const PlaceableElement* element = ChooseRandomElement(m_params.m_placeableElements);
        if (element == nullptr || element->SpawnFunc == nullptr)
            return;

        float elementScale = Rng::Float(element->m_minScale, element->m_maxScale);

        float extend = min(scale.x * m_params.m_width * 0.5f, scale.y * m_params.m_height * 0.5f);
        XMFLOAT3 spawnExtents = {
            extend,
            extend,
            0.1f
        };

        PoissonDisc poissonDisc(element->m_minRadiusCluster, { 0,0,0 }, spawnExtents);

        int numPoints = Rng::Int(element->m_minCount, element->m_maxCount);

        Vector<XMFLOAT3> points = poissonDisc.Generate(numPoints);

        std::shuffle(points.begin(), points.end(), Rng::GetGen());

        int pointsToKeep = (int)(points.size() * min(1.f, m_params.m_spawnDensity));
        points.resize(pointsToKeep);

        TransformComponent& transform = wall->GetComponent<TransformComponent>();
        transform.UpdateWorldData();
        XMFLOAT4X4 worldMatrix = transform.GetWorldMatrix();
        XMMATRIX mWorldMatrix = XMLoadFloat4x4(&worldMatrix);

        for (const auto& localPoint : points)
        {
            XMVECTOR vLocal = XMLoadFloat3(&localPoint);
            XMVECTOR vWorld = XMVector3Transform(vLocal, mWorldMatrix);
            XMFLOAT3 spawnPos;
            XMStoreFloat3(&spawnPos, vWorld);

            float halfWidth = scale.x * m_params.m_width * 0.5f;
            float halfHeight = scale.y * m_params.m_height * 0.5f;

            if (fabs(localPoint.x) <= halfWidth && fabs(localPoint.y) <= halfHeight)
            {
                GameObject* spawnedElement = element->SpawnFunc(mp_scene, spawnPos, normal, elementScale);
                caveTubeSection.m_gameObjects.push_back(spawnedElement);
                spawnedElement->SetActive(true);
            }
        }

        return;
    }

    const PlaceableElement* CaveSections::ChooseRandomElement(const Vector<PlaceableElement>& elements)
    {
        if (elements.empty()) return nullptr;

        float totalWeight = 0.0f;
        for (const auto& element : elements)
            totalWeight += element.m_weight;

        float random = Rng::Float(0.0f, totalWeight);

        float cumulativeWeight = 0.0f;
        for (const auto& element : elements)
        {
            cumulativeWeight += element.m_weight;
            if (random <= cumulativeWeight)
                return &element;
        }

        return &elements.back();
    }

    void CaveSections::GenerateWallMeshes(u8 count)
    {
        for (int i = 0; i < count; i++)
        {
            VoxelGrid voxelWall;
            XMFLOAT3 wallSize = { m_params.m_width, m_params.m_height, m_params.m_depth };
            voxelWall.MakeVoxelBox(m_params.m_voxelSize, wallSize);

            Mesh* mesh = RENDER_MANAGER->GetRenderItemManager().CreateRenderItem<Mesh>();
            mesh->BuildFromVoxels(voxelWall, m_params.m_color);

            if ((int)m_params.m_textureTag != -1)
                mesh->SetTexture((int)m_params.m_textureTag);

            mesh->MakeNoise(m_params.m_noiseOctaves, m_params.m_noiseFrequency, m_params.m_noiseAmplitude);

            m_wallMeshes.push_back(mesh);
        }
    }

    void CaveSections::GenerateFloorMesh()
    {
        VoxelGrid voxelWall;
        XMFLOAT3 wallSize = { m_params.m_width, m_params.m_height, m_params.m_depth };
        voxelWall.MakeVoxelBox(m_params.m_voxelSize, wallSize);

        Mesh* mesh = RENDER_MANAGER->GetRenderItemManager().CreateRenderItem<Mesh>();
        mesh->BuildFromVoxels(voxelWall, m_params.m_color);

        if ((int)m_params.m_textureTag != -1)
            mesh->SetTexture((int)m_params.m_textureTag);

        m_floorMesh = mesh;
    }
}