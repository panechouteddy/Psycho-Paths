#include "pch.h"
#include "CaveGenerator.h"
#include "Rng.h"

using namespace DirectX;

namespace nam
{
    CaveGenerator::CaveGenerator(Scene* scene, const CaveSectionParams& sectionParams, u8 meshCount)
        : mp_scene(scene)
    {
        mp_caveSections = new CaveSections(scene, sectionParams);
        mp_caveSections->GenerateMeshes(meshCount);
    }

    CaveGenerator::~CaveGenerator()
    {
        if (mp_caveSections)
        {
            delete mp_caveSections;
            mp_caveSections = nullptr;
        }
    }

    void CaveGenerator::Generate(const CaveGeneratorParams& params, const XMFLOAT3& start, std::optional<XMFLOAT3> end)
    {
        Rng::SetSeed(params.m_seed);

        mp_caveSections->BeginGeneration();

        XMFLOAT3 actualStart = start;
        XMFLOAT3 actualEnd;

        if (end.has_value())
        {
            actualEnd = end.value();
        }
        else
        {
            float angle = Rng::Float(-XM_PI, XM_PI);
            float pitch = Rng::Float(-0.10f, 0.10f);

            XMFLOAT3 dir;
            dir.x = cosf(angle) * cosf(pitch);
            dir.y = sinf(pitch);
            dir.z = sinf(angle) * cosf(pitch);

            XMVECTOR vDir = XMLoadFloat3(&dir);
            vDir = XMVector3Normalize(vDir);
            XMStoreFloat3(&dir, vDir);

            float tunnelLength = params.m_tunnelLength;

            XMStoreFloat3(&actualEnd, XMVectorAdd(XMLoadFloat3(&actualStart), XMVectorScale(vDir, tunnelLength)));
        }

        GenerateTunnel(actualStart, actualEnd, params.m_tunnelRadius);
    }

    void CaveGenerator::PopGeneration()
    {
        mp_caveSections->PopGeneration();
    }

    Vector<CaveTubeSection>* CaveGenerator::GetLastTubeSections()
    {
        return mp_caveSections->GetLastCaveTubeSections();
    }

    void CaveGenerator::GenerateTunnel(const XMFLOAT3& start, const XMFLOAT3& end, float radius)
    {
        XMVECTOR vStart = XMLoadFloat3(&start);
        XMVECTOR vEnd = XMLoadFloat3(&end);
        XMVECTOR totalDir = XMVectorSubtract(vEnd, vStart);
        float totalLength = XMVectorGetX(XMVector3Length(totalDir));

        float segmentLength = radius * 3.0f;

        Vector<XMVECTOR> controlPoints;
        controlPoints.push_back(vStart);

        XMVECTOR startDir = XMVector3Normalize(totalDir);

        int numControlPoints = 5;
        for (int i = 1; i <= numControlPoints; i++)
        {
            float t = (float)i / (numControlPoints + 1);

            float influence = sinf(t * XM_PI);

            float angleYaw = sinf(t * XM_2PI * 0.6f) * Rng::Float(-0.6f, 0.6f) * influence;
            float anglePitch = sinf(t * XM_2PI * 0.4f) * Rng::Float(-0.4f, 0.4f) * influence;

            XMVECTOR vUp = XMVectorSet(0, 1, 0, 0);
            XMMATRIX rotYaw = XMMatrixRotationAxis(vUp, angleYaw);

            XMVECTOR vRight = XMVector3Normalize(XMVector3Cross(vUp, startDir));
            XMMATRIX rotPitch = XMMatrixRotationAxis(vRight, anglePitch);

            XMVECTOR dirAtT = XMVector3Transform(startDir, rotYaw);
            dirAtT = XMVector3Transform(dirAtT, rotPitch);
            dirAtT = XMVector3Normalize(dirAtT);

            XMVECTOR point = XMVectorAdd(vStart, XMVectorScale(totalDir, t));

            XMVECTOR deviation = XMVectorScale(dirAtT, totalLength * 0.2f * influence);
            point = XMVectorAdd(point, deviation);

            controlPoints.push_back(point);
        }
        controlPoints.push_back(vEnd);

        XMVECTOR lastDir = XMVector3Normalize(XMVectorSubtract(vEnd, controlPoints[controlPoints.size() - 2]));
        XMVECTOR secondLast = controlPoints[controlPoints.size() - 2];
        XMVECTOR adjustedPoint = XMVectorSubtract(vEnd, XMVectorScale(lastDir, totalLength * 0.1f));
        controlPoints[controlPoints.size() - 2] = adjustedPoint;

        Vector<XMFLOAT3> tunnelPoints;
        tunnelPoints.push_back(start);

        float accumulatedLength = 0.0f;
        XMVECTOR lastPoint = vStart;

        const int STEPS = (int)(totalLength / segmentLength) * 3;

        for (int i = 1; i <= STEPS; i++)
        {
            float t = (float)i / STEPS;

            float segmentT = t * (controlPoints.size() - 1);
            int idx = (int)segmentT;
            float localT = segmentT - idx;

            XMVECTOR point;
            if (idx == 0)
            {
                XMVECTOR p0 = controlPoints[0];
                XMVECTOR p1 = controlPoints[1];
                XMVECTOR p2 = controlPoints[2];
                point = XMVectorLerp(p0, p1, localT);
            }
            else if (idx >= controlPoints.size() - 2)
            {
                XMVECTOR p0 = controlPoints[controlPoints.size() - 2];
                XMVECTOR p1 = controlPoints[controlPoints.size() - 1];

                float endInfluence = (float)(idx - (controlPoints.size() - 3)) / 2.0f;
                endInfluence = min(1.0f, max(0.0f, endInfluence));

                XMVECTOR dirToEnd = XMVector3Normalize(XMVectorSubtract(vEnd, p0));
                XMVECTOR idealPoint = XMVectorAdd(p0, XMVectorScale(dirToEnd, XMVectorGetX(XMVector3Length(XMVectorSubtract(p1, p0)))));

                point = XMVectorLerp(XMVectorLerp(p0, p1, localT), idealPoint, endInfluence * localT);
            }
            else
            {
                XMVECTOR p0 = controlPoints[idx - 1];
                XMVECTOR p1 = controlPoints[idx];
                XMVECTOR p2 = controlPoints[idx + 1];
                XMVECTOR p3 = controlPoints[idx + 2];
                point = XMVectorCatmullRom(p0, p1, p2, p3, localT);
            }

            XMVECTOR delta = XMVectorSubtract(point, lastPoint);
            float dist = XMVectorGetX(XMVector3Length(delta));
            accumulatedLength += dist;

            if (accumulatedLength >= segmentLength)
            {
                XMFLOAT3 p;
                XMStoreFloat3(&p, point);
                tunnelPoints.push_back(p);
                lastPoint = point;
                accumulatedLength = 0.0f;
            }
        }

        XMFLOAT3 lastTunnelPoint = tunnelPoints.back();
        XMVECTOR vLast = XMLoadFloat3(&lastTunnelPoint);
        float distToEnd = XMVectorGetX(XMVector3Length(XMVectorSubtract(vEnd, vLast)));

        if (distToEnd > 0.5f)
        {
            tunnelPoints.push_back(end);
        }
        else
        {
            tunnelPoints.back() = end;
        }

        for (size i = 0; i < tunnelPoints.size() - 1; i++)
        {
            mp_caveSections->CreateTube(tunnelPoints[i], tunnelPoints[i + 1], radius);
        }
    }
}