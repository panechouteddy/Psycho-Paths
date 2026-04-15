#include "pch.h"
#include "PointHasher.h"

using namespace DirectX;

namespace nam
{
	size PointHasher::Hash(int x, int y, int z)
	{
		return (x * HASH_X) ^ (y * HASH_Y) ^ (z * HASH_Z);
	}

	size PointHasher::Hash(XMFLOAT3 xyz)
	{
		int x = static_cast<int>(std::floor(xyz.x * INV_GRID_HASH_EPSILON));
		int y = static_cast<int>(std::floor(xyz.y * INV_GRID_HASH_EPSILON));
		int z = static_cast<int>(std::floor(xyz.z * INV_GRID_HASH_EPSILON));
		return Hash(x, y, z);
	}
}
