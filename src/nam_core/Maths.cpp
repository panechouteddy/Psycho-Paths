#include "pch.h"
#include "Maths.h"

using namespace DirectX;

namespace nam
{
	bool Maths::XMFLOAT3IsEqual(DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2, float margin)
	{
		return (p1.x >= p2.x - margin && p1.x <= p2.x + margin) &&
			   (p1.y >= p2.y - margin && p1.y <= p2.y + margin) &&
			   (p1.z >= p2.z - margin && p1.z <= p2.z + margin);
	}
	float Maths::GetSquareDistance(DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2)
	{
		XMVECTOR dv = XMVectorSubtract(XMLoadFloat3(&p2), XMLoadFloat3(&p1));

		XMFLOAT3 d = {};
		XMStoreFloat3(&d, dv);

		return d.x*d.x + d.y*d.y + d.z*d.z;
	}
	DirectX::XMFLOAT3 Maths::GetDirection(DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2)
	{
		XMFLOAT3 dir = {};

		XMStoreFloat3( &dir, XMVector3Normalize( XMVectorSubtract(XMLoadFloat3(&p2), XMLoadFloat3(&p1)) ) );

		return dir;
	}

	void Maths::SelfNormalize(DirectX::XMFLOAT3& v)
	{
		XMVECTOR vec = XMLoadFloat3(&v);
		XMVector3Normalize(vec);

		XMStoreFloat3(&v, vec);
	}
}