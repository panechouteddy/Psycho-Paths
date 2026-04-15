#pragma once
namespace nam
{
	struct ParticleEmitersComponent
	{
		Vector<DirectX::XMFLOAT3> m_maxXYZ;
		Vector<DirectX::XMFLOAT3> m_minXYZ;

		Vector<DirectX::XMFLOAT3> m_maxDir;
		Vector<DirectX::XMFLOAT3> m_minDir;

		Vector<DirectX::XMFLOAT3> m_startColor;
		Vector<DirectX::XMFLOAT3> m_endColor;

		Vector<float> m_maxSpeed;
		Vector<float> m_minSpeed;

		Vector<float> m_lifeTime;

		Vector<float> m_spawnRate;
		Vector<float> m_spawnAdvancement;

		Vector<bool> m_isActive;

		void AddEmiter(
			DirectX::XMFLOAT3 maxXYZ,
			DirectX::XMFLOAT3 minXYZ,
			DirectX::XMFLOAT3 maxDir,
			DirectX::XMFLOAT3 minDir,
			DirectX::XMFLOAT3 startColor,
			DirectX::XMFLOAT3 endColor,
			float maxSpeed,
			float minSpeed,
			float lifeTime,
			float spawnRate
		);
	};
}

