#pragma once
namespace nam
{
	struct CaveTubeSection
	{
		DirectX::XMFLOAT3 m_start = {};
		DirectX::XMFLOAT3 m_end = {};
		float m_radius = 0.f;
		Vector<GameObject*> m_gameObjects = {};
	};
}


