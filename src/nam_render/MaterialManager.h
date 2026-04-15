#pragma once
#include <ContainerSequential.h>
#include "Constants.h"
#include "DataShader.h"

namespace nam
{
	class MaterialManager
	{
		UnMap<size, CBufferMaterial> m_taggedMaterials = {};

	public:
		MaterialManager();
		~MaterialManager();

		void CreateMaterial(size uniqueTag, float roughness, float metalness, DirectX::XMFLOAT3 albedo = { 1, 1, 1 });

		//Keep -1 if you don't want to change this parameter
		void ChangeMaterial(size tag, float roughness = -1, float metalness = -1, DirectX::XMFLOAT3 albedo = { -1, -1, -1 });
	};
}

