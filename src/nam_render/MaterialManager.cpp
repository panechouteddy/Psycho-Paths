#include "pch.h"
#include "MaterialManager.h"

#include "RenderManager.h"

namespace nam
{
	MaterialManager::MaterialManager()
	{
		m_taggedMaterials.reserve(MAX_MATERIALS);
	}

	MaterialManager::~MaterialManager()
	{
		m_taggedMaterials.clear();
	}

	void MaterialManager::CreateMaterial(size uniqueTag, float roughness, float metalness, DirectX::XMFLOAT3 albedo)
	{
		auto it = m_taggedMaterials.find(uniqueTag);

		if (it != m_taggedMaterials.end())
		{
			_ASSERT(false && "CreateMaterial() -> This Material already Exists !");
			return;
		}

		m_taggedMaterials[uniqueTag] = CBufferMaterial();

		CBufferMaterial& mat = m_taggedMaterials[uniqueTag];
		mat.m_albedo = albedo;
		mat.m_roughness = roughness;
		mat.m_metalness = metalness;

		RENDER_MANAGER->GetConstantBuffersDatas()->mp_matCB->CopyData((int)uniqueTag, mat);
	}
	void MaterialManager::ChangeMaterial(size tag, float roughness, float metalness, DirectX::XMFLOAT3 albedo)
	{
		auto it = m_taggedMaterials.find(tag);

		if (it == m_taggedMaterials.end())
		{
			_ASSERT(false && "ChangeMaterial() -> This Material doesn't Exist !");
			return;
		}

		CBufferMaterial& mat = it->second;

		if(albedo.x != -1)
			mat.m_albedo = albedo;

		if(roughness != -1)
			mat.m_roughness = roughness;

		if(metalness != -1)
			mat.m_metalness = metalness;

		RENDER_MANAGER->GetConstantBuffersDatas()->mp_matCB->CopyData((int)tag, mat);
	}
}
