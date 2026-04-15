#include "pch.h"
#include "VoxelGrid.h"

using namespace DirectX;

namespace nam
{
    VoxelGrid::VoxelGrid(int w, int h, int d, float size)
    {
        Init(w, h, d, size);
    }

    void VoxelGrid::Init(int w, int h, int d, float size)
    {
        width = w;
        height = h;
        depth = d;
        voxelSize = size;
        voxels.resize(w * h * d, false);
    }

    bool VoxelGrid::GetAt(int x, int y, int z) const
    {
        if (x >= 0 && x < width && y >= 0 && y < height && z >= 0 && z < depth)
        {
            return voxels[z * width * height + y * width + x];
        }
        return false;
    }

    void VoxelGrid::SetAt(int x, int y, int z, bool val)
    {
        if (x >= 0 && x < width && y >= 0 && y < height && z >= 0 && z < depth)
        {
            voxels[z * width * height + y * width + x] = val;
        }
    }

    void VoxelGrid::MakeHole(XMFLOAT3 from, XMFLOAT3 to, float radius)
    {
        XMVECTOR vFrom = XMLoadFloat3(&from);
        XMVECTOR vTo = XMLoadFloat3(&to);
        XMVECTOR direction = XMVectorSubtract(vTo, vFrom);
        float length = XMVectorGetX(XMVector3Length(direction));

        if (length < 0.001f)
            return;

        XMVECTOR unitDir = XMVector3Normalize(direction);
        float radiusSq = radius * radius;

        float worldWidth = width * voxelSize;
        float worldHeight = height * voxelSize;
        float worldDepth = depth * voxelSize;

        XMFLOAT3 worldCenter = { worldWidth * 0.5f, worldHeight * 0.5f, worldDepth * 0.5f };
        XMVECTOR vWorldCenter = XMLoadFloat3(&worldCenter);

        for (int z = 0; z < depth; z++)
        {
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    XMFLOAT3 voxelPos(
                        x * voxelSize - worldCenter.x,
                        y * voxelSize - worldCenter.y,
                        z * voxelSize - worldCenter.z
                    );

                    XMVECTOR vVoxel = XMLoadFloat3(&voxelPos);
                    XMVECTOR voxelToStart = XMVectorSubtract(vVoxel, vFrom);

                    float projection = XMVectorGetX(XMVector3Dot(voxelToStart, unitDir));

                    if (projection >= 0 && projection <= length)
                    {
                        XMVECTOR pointOnAxis = XMVectorAdd(vFrom, XMVectorScale(unitDir, projection));
                        XMVECTOR toVoxel = XMVectorSubtract(vVoxel, pointOnAxis);
                        float distSq = XMVectorGetX(XMVector3LengthSq(toVoxel));

                        if (distSq <= radiusSq)
                        {
                            SetAt(x, y, z, false);
                        }
                    }
                }
            }
        }
    }

    void VoxelGrid::MakeHole(XMFLOAT3 from, XMFLOAT3 to, XMFLOAT3& box)
    {
        XMVECTOR vFrom = XMLoadFloat3(&from);
        XMVECTOR vTo = XMLoadFloat3(&to);
        XMVECTOR vBox = XMLoadFloat3(&box);

        XMVECTOR direction = XMVectorSubtract(vTo, vFrom);
        float length = XMVectorGetX(XMVector3Length(direction));

        if (length < 0.001f)
            return;

        XMVECTOR unitDir = XMVector3Normalize(direction);
        XMVECTOR halfBox = XMVectorScale(vBox, 0.5f);

        float worldWidth = width * voxelSize;
        float worldHeight = height * voxelSize;
        float worldDepth = depth * voxelSize;

        XMFLOAT3 worldCenter = { worldWidth * 0.5f, worldHeight * 0.5f, worldDepth * 0.5f };
        XMVECTOR vWorldCenter = XMLoadFloat3(&worldCenter);

        for (int z = 0; z < depth; z++)
        {
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    XMFLOAT3 voxelPos(
                        x * voxelSize - worldCenter.x,
                        y * voxelSize - worldCenter.y,
                        z * voxelSize - worldCenter.z
                    );

                    XMVECTOR vVoxel = XMLoadFloat3(&voxelPos);
                    XMVECTOR voxelToStart = XMVectorSubtract(vVoxel, vFrom);

                    float projection = XMVectorGetX(XMVector3Dot(voxelToStart, unitDir));

                    if (projection >= 0 && projection <= length)
                    {
                        XMVECTOR basePoint = XMVectorAdd(vFrom, XMVectorScale(unitDir, projection));

                        XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
                        XMVECTOR right = XMVector3Cross(unitDir, up);

                        if (XMVectorGetX(XMVector3LengthSq(right)) < 0.001f)
                        {
                            up = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
                            right = XMVector3Cross(unitDir, up);
                        }

                        right = XMVector3Normalize(right);
                        XMVECTOR forward = XMVector3Normalize(XMVector3Cross(right, unitDir));

                        XMVECTOR localPos = XMVectorSubtract(vVoxel, basePoint);
                        float coord1 = XMVectorGetX(XMVector3Dot(localPos, right));
                        float coord2 = XMVectorGetX(XMVector3Dot(localPos, forward));

                        if (fabs(coord1) <= XMVectorGetX(halfBox) &&
                            fabs(coord2) <= XMVectorGetY(halfBox))
                        {
                            SetAt(x, y, z, false);
                        }
                    }
                }
            }
        }
    }

    void VoxelGrid::MakeVoxelSphere(float _voxelSize, float radius)
    {
        int diameter = static_cast<int>(ceil(2.0f * radius / _voxelSize));
        int center = diameter / 2;
        float radiusSq = radius * radius;

        Init(diameter, diameter, diameter, _voxelSize);

        for (int z = 0; z < diameter; z++)
        {
            float wz = (z - center) * _voxelSize;
            for (int y = 0; y < diameter; y++)
            {
                float wy = (y - center) * _voxelSize;
                for (int x = 0; x < diameter; x++)
                {
                    float wx = (x - center) * _voxelSize;
                    float distSq = wx * wx + wy * wy + wz * wz;

                    if (distSq <= radiusSq)
                    {
                        SetAt(x, y, z, true);
                    }
                }
            }
        }
    }

    void VoxelGrid::MakeVoxelBox(float _voxelSize, XMFLOAT3& size)
    {
        int width = static_cast<int>(ceil(size.x / _voxelSize));
        int height = static_cast<int>(ceil(size.y / _voxelSize));
        int depth = static_cast<int>(ceil(size.z / _voxelSize));

        Init(width, height, depth, _voxelSize);

        for (int z = 0; z < depth; z++)
        {
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    SetAt(x, y, z, true);
                }
            }
        }
    }

}
