#pragma once
#include <DirectXMath.h>

namespace nam
{
	struct Maths
	{
		//Compare
		static bool XMFLOAT3IsEqual(DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2, float margin = 0.01f);

		//Return the distance between 2 points at the power 2? Use Sqrt to have the real distance
		static float GetSquareDistance(DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2);

		//Return the normalized direction
		static DirectX::XMFLOAT3 GetDirection(DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2);

		static void SelfNormalize(DirectX::XMFLOAT3& v);
	};
}

