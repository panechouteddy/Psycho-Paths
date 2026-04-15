#include "pch.h"
#include "PoissonDisc.h"
#include "Rng.h"

using namespace DirectX;

namespace nam
{
    PoissonDisc::PoissonDisc(float minDist, const XMFLOAT3& center, const XMFLOAT3& extents, int attempts)
        : m_minDistance(minDist), m_center(center), m_extents(extents), m_maxAttempts(attempts)
    {
        m_cellSize = m_minDistance / 1.732050808f; // sqrt(3)
        m_invertCellSize = 1.f / m_cellSize;

        float totalWidth = extents.x * 2.0f;
        float totalHeight = extents.y * 2.0f;
        float totalDepth = extents.z * 2.0f;

        m_gridWidth = static_cast<int>(std::ceil(totalWidth * m_invertCellSize)) + 1;
        m_gridHeight = static_cast<int>(std::ceil(totalHeight * m_invertCellSize)) + 1;
        m_gridDepth = static_cast<int>(std::ceil(totalDepth * m_invertCellSize)) + 1;

        m_grid.resize(m_gridWidth * m_gridHeight * m_gridDepth);
    }

    Vector<XMFLOAT3> PoissonDisc::Generate(int numPoints, XMFLOAT3 seedPoint)
    {
        m_finalPoints.clear();
        m_activeList.clear();

        if (seedPoint.x == 0.f && seedPoint.y == 0.f && seedPoint.z == 0.f)
        {
            seedPoint.x = Rng::Float(-1.f, 1.f) * m_extents.x;
            seedPoint.y = Rng::Float(-1.f, 1.f) * m_extents.y;
            seedPoint.z = Rng::Float(-1.f, 1.f) * m_extents.z;
        }

        AddPoint(seedPoint);

        while (!m_activeList.empty())
        {
            int activeIndex = static_cast<int>(Rng::Float() * m_activeList.size());
            XMFLOAT3 currentPoint = m_activeList[activeIndex];

            bool found = false;

            for (int attempt = 0; attempt < m_maxAttempts; ++attempt)
            {
                XMFLOAT3 candidate = GenerateRandomPointAround(currentPoint);

                if (!(candidate.x >= -m_extents.x && candidate.x <= m_extents.x &&
                    candidate.y >= -m_extents.y && candidate.y <= m_extents.y &&
                    candidate.z >= -m_extents.z && candidate.z <= m_extents.z))
                {
                    continue;
                }

                if (IsValid(candidate))
                {
                    AddPoint(candidate);
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                m_activeList.erase(m_activeList.begin() + activeIndex);
            }

            if (numPoints > 0 && m_finalPoints.size() >= static_cast<size_t>(numPoints))
            {
                break;
            }
        }

        return m_finalPoints;
    }

    const Vector<XMFLOAT3>& PoissonDisc::GetPoints() const
    {
        return m_finalPoints;
    }

    void PoissonDisc::AddPoint(const XMFLOAT3& point)
    {
        m_finalPoints.push_back(point);
        m_activeList.push_back(point);

        float gridX = point.x - (m_center.x - m_extents.x);
        float gridY = point.y - (m_center.y - m_extents.y);
        float gridZ = point.z - (m_center.z - m_extents.z);

        int gx = static_cast<int>(gridX * m_invertCellSize);
        int gy = static_cast<int>(gridY * m_invertCellSize);
        int gz = static_cast<int>(gridZ * m_invertCellSize);

        gx = std::clamp(gx, 0, m_gridWidth - 1);
        gy = std::clamp(gy, 0, m_gridHeight - 1);
        gz = std::clamp(gz, 0, m_gridDepth - 1);

        size index = gx + gy * m_gridWidth + gz * m_gridHeight * m_gridDepth;

        m_grid[index].hasPoint = true;
        m_grid[index].point = point;
    }

    bool PoissonDisc::IsValid(const XMFLOAT3& candidate)
    {
        float gridX = candidate.x - (m_center.x - m_extents.x);
        float gridY = candidate.y - (m_center.y - m_extents.y);
        float gridZ = candidate.z - (m_center.z - m_extents.z);

        int gx = static_cast<int>(gridX * m_invertCellSize);
        int gy = static_cast<int>(gridY * m_invertCellSize);
        int gz = static_cast<int>(gridZ * m_invertCellSize);

        for (int dx = -1; dx <= 1; ++dx)
        {
            for (int dy = -1; dy <= 1; ++dy)
            {
                for (int dz = -1; dz <= 1; ++dz)
                {
                    int nx = gx + dx;
                    int ny = gy + dy;
                    int nz = gz + dz;

                    if (nx >= 0 && nx < m_gridWidth &&
                        ny >= 0 && ny < m_gridHeight &&
                        nz >= 0 && nz < m_gridDepth)
                    {
                        size index = nx + ny * m_gridWidth + nz * m_gridHeight * m_gridDepth;

                        PoissonDiscCell& cell = m_grid[index];
                        if (cell.hasPoint)
                        {
                            float dx = candidate.x - cell.point.x;
                            float dy = candidate.y - cell.point.y;
                            float dz = candidate.z - cell.point.z;

                            float distSq = dx * dx + dy * dy + dz * dz;

                            if (distSq < m_minDistance * m_minDistance)
                            {
                                return false;
                            }
                        }
                    }
                }
            }
        }

        return true;
    }


    XMFLOAT3 PoissonDisc::GenerateRandomPointAround(const XMFLOAT3& center)
    {
        float minExtent = min( m_extents.x, min(m_extents.y, m_extents.z));
        float threshold = minExtent * 0.1f;

        bool xFlat = m_extents.x < threshold;
        bool yFlat = m_extents.y < threshold;
        bool zFlat = m_extents.z < threshold;

        int flatCount = (xFlat ? 1 : 0) + (yFlat ? 1 : 0) + (zFlat ? 1 : 0);

        float theta = Rng::Float(0.f, XM_2PI);
        float r = m_minDistance * (1.0f + Rng::Float());

        XMFLOAT3 offset = { 0,0,0 };

        if (flatCount >= 2)
        {
            if (!xFlat)
            {
                float sign = Rng::Float() > 0.5f ? 1.0f : -1.0f;
                offset.x = r * sign;
            }
            else if (!yFlat)
            {
                float sign = Rng::Float() > 0.5f ? 1.0f : -1.0f;
                offset.y = r * sign;
            }
            else
            {
                float sign = Rng::Float() > 0.5f ? 1.0f : -1.0f;
                offset.z = r * sign;
            }
        }
        else if (flatCount == 1) 
        {
            if (yFlat)
            {
                offset.x = r * std::cos(theta);
                offset.z = r * std::sin(theta);
            }
            else if (xFlat) 
            {
                offset.y = r * std::cos(theta);
                offset.z = r * std::sin(theta);
            }
            else if (zFlat)
            {
                offset.x = r * std::cos(theta);
                offset.y = r * std::sin(theta);
            }
        }
        else 
        {
            float phi = std::acos(Rng::Float(-1.0f, 1.0f));
            offset.x = r * std::sin(phi) * std::cos(theta);
            offset.y = r * std::sin(phi) * std::sin(theta);
            offset.z = r * std::cos(phi);
        }

        return XMFLOAT3(
            center.x + offset.x,
            center.y + offset.y,
            center.z + offset.z
        );
    }
}