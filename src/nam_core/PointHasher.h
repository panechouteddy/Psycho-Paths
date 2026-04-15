#pragma once
#include "DirectXMath.h"

namespace nam
{
	class PointHasher
	{
	private:
		static constexpr u64 HASH_X = 73856093;
		static constexpr u64 HASH_Y = 19349663;
		static constexpr u64 HASH_Z = 83492791;
		static constexpr float INV_GRID_HASH_EPSILON = 1.f / 0.001f;

	public:
		static size Hash(int x, int y, int z);
		static size Hash(DirectX::XMFLOAT3 xyz);
	};
}


