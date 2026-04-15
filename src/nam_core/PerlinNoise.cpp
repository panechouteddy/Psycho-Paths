#include "pch.h"
#include "PerlinNoise.h"
#include "Rng.h"

using namespace DirectX;

namespace nam
{
	XMFLOAT3 PerlinNoise::ms_gradiants[12] = {
		{1,1,0},{-1,1,0},{1,-1,0},{-1,-1,0},
		{1,0,1},{-1,0,1},{1,0,-1},{-1,0,-1},
		{0,1,1},{0,-1,1},{0,1,-1},{0,-1,-1}
	};

	int PerlinNoise::ms_permutaions[256];

	float PerlinNoise::Make(XMFLOAT3 point)
	{
		float x = point.x;
		float y = point.y;
		float z = point.z;

		int x0 = (int)floor(x);
		int x1 = x0 + 1;
		int y0 = (int)floor(y);
		int y1 = y0 + 1;
		int z0 = (int)floor(z);
		int z1 = z0 + 1;

		float sx = x - x0;
		float sy = y - y0;
		float sz = z - z0;

		float n000 = DotGradiant(x0, y0, z0, x, y, z);
		float n100 = DotGradiant(x1, y0, z0, x, y, z);
		float ix00 = Interpolate(n000, n100, sx);

		float n010 = DotGradiant(x0, y1, z0, x, y, z);
		float n110 = DotGradiant(x1, y1, z0, x, y, z);
		float ix10 = Interpolate(n010, n110, sx);

		float n001 = DotGradiant(x0, y0, z1, x, y, z);
		float n101 = DotGradiant(x1, y0, z1, x, y, z);
		float ix01 = Interpolate(n001, n101, sx);

		float n011 = DotGradiant(x0, y1, z1, x, y, z);
		float n111 = DotGradiant(x1, y1, z1, x, y, z);
		float ix11 = Interpolate(n011, n111, sx);

		float iy0 = Interpolate(ix00, ix10, sy);
		float iy1 = Interpolate(ix01, ix11, sy);
		float noise = Interpolate(iy0, iy1, sz);

		return noise;
	}

	void PerlinNoise::GeneratePermutations()
	{
		for (int i = 0; i < 256; i++)
		{
			ms_permutaions[i] = (int)i;
		}

		std::shuffle(&ms_permutaions[0], &ms_permutaions[255], Rng::GetGen());
	}


	float PerlinNoise::Fade(float w)
	{
		return w * w * w * (w * (w * 6 - 15) + 10);
	}

	float PerlinNoise::Interpolate(float a0, float a1, float w)
	{
		return a0 + Fade(w) * (a1 - a0);
	}

	float PerlinNoise::DotGradiant(int ix, int iy, int iz, float x, float y, float z) {
		int h = ms_permutaions[(ix + ms_permutaions[(iy + ms_permutaions[iz & 255]) & 255]) & 255];
		XMFLOAT3 g = ms_gradiants[h % 12];
		float dx = x - ix;
		float dy = y - iy;
		float dz = z - iz;
		return dx * g.x + dy * g.y + dz * g.z;
	}
}

