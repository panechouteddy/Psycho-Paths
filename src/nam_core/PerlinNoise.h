#pragma once
#include "DirectXMath.h"

namespace nam
{
	class PerlinNoise
	{
	private:
		static DirectX::XMFLOAT3 ms_gradiants[12];
		static int ms_permutaions[256];

	public:
		static float Make(DirectX::XMFLOAT3 point);
		static void GeneratePermutations();

	private:
		static float Fade(float w);
		static float Interpolate(float a0, float a1, float w);
		static float DotGradiant(int ix, int iy, int iz, float x, float y, float z);
	};
}


