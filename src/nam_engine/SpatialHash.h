#pragma once

namespace nam
{
    class SpatialHash
    {
    private:
        UnMap<size_t, Vector<u32>> grid;
        float cellSize;
        float invCellSize;

    public:
        SpatialHash(float size = 10.0f);

        void Clear();

        void Insert(u32 entity, const DirectX::BoundingOrientedBox& box);
        void Insert(u32 entity, const DirectX::BoundingSphere& sphere);

        void GetNearby(u32 entity, const DirectX::BoundingOrientedBox& box, Vector<u32>& result);
        void GetNearby(u32 entity, const DirectX::BoundingSphere& sphere, Vector<u32>& result);
    };

}
