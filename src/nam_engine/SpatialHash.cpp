#include "pch.h"
#include "SpatialHash.h"
#include "PointHasher.h"

using namespace DirectX;

namespace nam
{
	SpatialHash::SpatialHash(float size) : cellSize(size), invCellSize(1.0f / size)
	{
	}

	void SpatialHash::Clear()
	{ 
		grid.clear();
	}

    void SpatialHash::Insert(u32 entity, const DirectX::BoundingOrientedBox& box)
    {
        DirectX::XMFLOAT3 corners[8];
        box.GetCorners(corners);

        int minX = INT_MAX;
        int maxX = INT_MIN;
        int minY = INT_MAX;
        int maxY = INT_MIN;
        int minZ = INT_MAX;
        int maxZ = INT_MIN;

        for (int i = 0; i < 8; i++) {
            int x = (int)floor(corners[i].x * invCellSize);
            int y = (int)floor(corners[i].y * invCellSize);
            int z = (int)floor(corners[i].z * invCellSize);

            minX = min(minX, x);
            maxX = max(maxX, x);
            minY = min(minY, y);
            maxY = max(maxY, y);
            minZ = min(minZ, z);
            maxZ = max(maxZ, z);
        }

        for (int x = minX; x <= maxX; x++) {
            for (int y = minY; y <= maxY; y++) {
                for (int z = minZ; z <= maxZ; z++) {
                    size h = PointHasher::Hash(x, y, z);
                    grid[h].push_back(entity);
                }
            }
        }
    }

    void SpatialHash::Insert(u32 entity, const DirectX::BoundingSphere& sphere)
    {
        int centerX = (int)floor(sphere.Center.x * invCellSize);
        int centerY = (int)floor(sphere.Center.y * invCellSize);
        int centerZ = (int)floor(sphere.Center.z * invCellSize);

        int radiusCells = (int)ceil(sphere.Radius * invCellSize);

        for (int dx = -radiusCells; dx <= radiusCells; dx++) {
            for (int dy = -radiusCells; dy <= radiusCells; dy++) {
                for (int dz = -radiusCells; dz <= radiusCells; dz++) {
                    size h = PointHasher::Hash(centerX + dx, centerY + dy, centerZ + dz);
                    grid[h].push_back(entity);
                }
            }
        }
    }

    void SpatialHash::GetNearby(u32 entity, const BoundingOrientedBox& box, Vector<u32>& result)
    {
        result.clear();
        result.reserve(128);
        DirectX::XMFLOAT3 corners[8];
        box.GetCorners(corners);
        int minX = INT_MAX, maxX = INT_MIN;
        int minY = INT_MAX, maxY = INT_MIN;
        int minZ = INT_MAX, maxZ = INT_MIN;

        for (int i = 0; i < 8; i++) {
            int x = (int)floor(corners[i].x * invCellSize);
            int y = (int)floor(corners[i].y * invCellSize);
            int z = (int)floor(corners[i].z * invCellSize);
            minX = min(minX, x); maxX = max(maxX, x);
            minY = min(minY, y); maxY = max(maxY, y);
            minZ = min(minZ, z); maxZ = max(maxZ, z);
        }
        UnSet<u32> uniqueEntities;
        for (int x = minX; x <= maxX; x++) {
            for (int y = minY; y <= maxY; y++) {
                for (int z = minZ; z <= maxZ; z++) {
                    size h = PointHasher::Hash(x, y, z);
                    auto it = grid.find(h);
                    if (it != grid.end()) {
                        for (u32 e : it->second) {
                            uniqueEntities.insert(e);
                        }
                    }
                }
            }
        }
        uniqueEntities.erase(entity);
        result.assign(uniqueEntities.begin(), uniqueEntities.end());
    }

    void SpatialHash::GetNearby(u32 entity, const BoundingSphere& sphere, Vector<u32>& result)
    {
        result.clear();
        result.reserve(128);

        int centerX = (int)floor(sphere.Center.x * invCellSize);
        int centerY = (int)floor(sphere.Center.y * invCellSize);
        int centerZ = (int)floor(sphere.Center.z * invCellSize);
        int radiusCells = (int)ceil(sphere.Radius * invCellSize);

        UnSet<u32> uniqueEntities;
        for (int dx = -radiusCells; dx <= radiusCells; dx++) {
            for (int dy = -radiusCells; dy <= radiusCells; dy++) {
                for (int dz = -radiusCells; dz <= radiusCells; dz++) {
                    size h = PointHasher::Hash(centerX + dx, centerY + dy, centerZ + dz);
                    auto it = grid.find(h);
                    if (it != grid.end()) {
                        for (u32 e : it->second) {
                            uniqueEntities.insert(e);
                        }
                    }
                }
            }
        }
        uniqueEntities.erase(entity);
        result.assign(uniqueEntities.begin(), uniqueEntities.end());
    }
}