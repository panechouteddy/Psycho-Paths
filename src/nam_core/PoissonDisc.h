#pragma once 
#include "DirectXMath.h"
#include "PoissonDiscCell.h"

namespace nam
{
    class PoissonDisc
    {
    private:
        float m_minDistance;
        float m_cellSize;
        float m_invertCellSize;
        int m_maxAttempts;

        DirectX::XMFLOAT3 m_center;
        DirectX::XMFLOAT3 m_extents;

        Vector<PoissonDiscCell> m_grid;
        Vector<DirectX::XMFLOAT3> m_activeList;
        Vector<DirectX::XMFLOAT3> m_finalPoints;

        int m_gridWidth;
        int m_gridHeight;
        int m_gridDepth;

    public:
        PoissonDisc(float minDist, const DirectX::XMFLOAT3& center, const DirectX::XMFLOAT3& extents, int attempts = 30);

        Vector<DirectX::XMFLOAT3> Generate(int numPoints = 0, DirectX::XMFLOAT3 seedPoint = {0.f,0.f,0.f});
        const Vector<DirectX::XMFLOAT3>& GetPoints() const;

    private:
        void AddPoint(const DirectX::XMFLOAT3& point);
        bool IsValid(const DirectX::XMFLOAT3& candidate);
        DirectX::XMFLOAT3 GenerateRandomPointAround(const DirectX::XMFLOAT3& center);
    };
}
