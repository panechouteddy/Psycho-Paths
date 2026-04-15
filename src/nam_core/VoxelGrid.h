#pragma once
#include <DirectXMath.h>
#include "ContainerSequential.h"

namespace nam
{
    struct VoxelGrid
    {
        Vector<bool> voxels;
        int width;
        int height;
        int depth;
        float voxelSize;

        VoxelGrid(int w = 1, int h = 1, int d = 1, float size = 1.f);
        void Init(int w, int h, int d, float size);

        bool GetAt(int x, int y, int z) const;
        void SetAt(int x, int y, int z, bool val);

        void MakeHole(DirectX::XMFLOAT3 from, DirectX::XMFLOAT3 to, float radius);
        void MakeHole(DirectX::XMFLOAT3 from, DirectX::XMFLOAT3 to, DirectX::XMFLOAT3& box);

        void MakeVoxelSphere(float _voxelSize, float radius);
        void MakeVoxelBox(float _voxelSize, DirectX::XMFLOAT3& size);
    };
}