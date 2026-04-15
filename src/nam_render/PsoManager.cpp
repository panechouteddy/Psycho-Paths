#include "pch.h"
#include "PsoManager.h"

namespace nam
{
	void PsoManager::BuildRootSignatures()
	{
		if (mp_rootSign2D == nullptr)
			mp_rootSign2D = RootSignature::Create(
				{ ConstantBufferView, ConstantBufferView },
				{ DescriptorTableDesc(ShaderResourceView, MAX_TEXTURES, 0) });
	}

	void PsoManager::BuildOthersRootSignatures()
	{
		if (mp_rootSign3D == nullptr)
			mp_rootSign3D = RootSignature::Create(
				{ ConstantBufferView, ConstantBufferView, ConstantBufferView, ConstantBufferView, ConstantBufferView },
				{ DescriptorTableDesc(ShaderResourceView, MAX_TEXTURES, 0) });

		if (mp_rootSignDrawParticle == nullptr)
			mp_rootSignDrawParticle = RootSignature::Create(
				{ ConstantBufferView },
				{ DescriptorTableDesc(ShaderResourceView, MAX_TEXTURES, 0) });

		if (mp_rootSignComputeParticleUpdate == nullptr)
			mp_rootSignComputeParticleUpdate = RootSignature::Create(
				{ ConstantBufferView },
				{
					DescriptorTableDesc(UnorderedAccessView, 1, 0),
					DescriptorTableDesc(UnorderedAccessView, 1, 1),
					DescriptorTableDesc(UnorderedAccessView, 1, 2)
				});

				if (mp_rootSignComputeParticleSpawn == nullptr)
					mp_rootSignComputeParticleSpawn = RootSignature::Create(
						{ ConstantBufferView },
			{
				DescriptorTableDesc(ShaderResourceView, 1, 0),
				DescriptorTableDesc(UnorderedAccessView, 1, 0),
				DescriptorTableDesc(UnorderedAccessView, 1, 1),
				DescriptorTableDesc(UnorderedAccessView, 1, 2)
			});
	}

	PsoManager::~PsoManager()
	{
		UnInit();
	}

	void PsoManager::UnInit()
	{
		m_pipelineStateBuilders.clear();

		if (mp_rootSign2D)
		{
			delete mp_rootSign2D;
			mp_rootSign2D = nullptr;
		}

		if (mp_rootSign3D)
		{
			delete mp_rootSign3D;
			mp_rootSign3D = nullptr;
		}

		if (mp_rootSignComputeParticleUpdate)
		{
			delete mp_rootSignComputeParticleUpdate;
			mp_rootSignComputeParticleUpdate = nullptr;
		}

		if (mp_rootSignComputeParticleSpawn)
		{
			delete mp_rootSignComputeParticleSpawn;
			mp_rootSignComputeParticleSpawn = nullptr;
		}

		for (auto& [key, shader] : m_shaders)
		{
			delete shader;
		}

		for (auto& [key, inputLayout] : m_inputLayouts)
		{
			delete inputLayout;
		}

		for (auto& [key, shader] : m_shaders)
		{
			delete shader;
		}

		m_shaders.clear();
		m_inputLayouts.clear();
	}

	void PsoManager::BuildMinimal()
	{
		BuildRootSignatures();
		Build2DTexture();
	}

	void PsoManager::BuildOthers()
	{
		BuildOthersRootSignatures();

		Build3DTexture();
		BuildParticleDraw();
		BuildParticleUpdate();
		BuildParticleSpawn();
	}

	void PsoManager::Build3DTexture()
	{
		m_shaders[PsoType::_Render3D] = Shader::Create("Texture3D", PATH_SHADER_TEXTURE_3D);

		InputLayout* p_IL = new InputLayout();

		p_IL->Add("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
		p_IL->Add("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12);
		p_IL->Add("TEXCOORDS", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28);
		p_IL->Add("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36);

		p_IL->Add("BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 48);
		p_IL->Add("BLENDWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 64);

		p_IL->Build();

		m_inputLayouts[PsoType::_Render3D] = p_IL;
		m_pipelineStateBuilders[PsoType::_Render3D].Build(m_shaders[PsoType::_Render3D], p_IL, mp_rootSign3D, false);
	}

	void PsoManager::Build2DTexture()
	{
		m_shaders[PsoType::_Render2D] = Shader::Create("Texture2D", PATH_SHADER_TEXTURE_2D);

		InputLayout* p_IL = new InputLayout();

		p_IL->Add("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
		p_IL->Add("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12);
		p_IL->Add("TEXCOORDS", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28);
		p_IL->Build();

		m_inputLayouts[PsoType::_Render2D] = p_IL;
		m_pipelineStateBuilders[PsoType::_Render2D].Build(m_shaders[PsoType::_Render2D], p_IL, mp_rootSign2D, true);
	}

	bool PsoManager::IsRootSignChangeUseless(RootSignatureName rootSignatureType)
	{
		return false;

		return m_currentRootSign == rootSignatureType && m_rootSignFirstBinded == true;

		//Optimization to not change the rootsignature if call the same root signature
	}

	RootSignature* PsoManager::GetRootSignature(RootSignatureName rootSignatureType)
	{
		m_rootSignFirstBinded = true;
		m_currentRootSign = rootSignatureType;
		switch (rootSignatureType)
		{
			case RootSignatureName::_Root2D:
				return mp_rootSign2D;
			case RootSignatureName::_Root3D:
				return mp_rootSign3D;
			case RootSignatureName::_RootDrawParticle:
				return mp_rootSignDrawParticle;
			case RootSignatureName::_RootUpdateParticle:
				return mp_rootSignComputeParticleUpdate;
			case RootSignatureName::_RootSpawnParticle:
				return mp_rootSignComputeParticleSpawn;
			default:
				return nullptr;
		}
	}

	void PsoManager::BuildParticleDraw()
	{
		m_shaders[PsoType::ParticleDraw] = Shader::Create("ParticleDraw", PATH_SHADER_PARTICLE_DRAW);

		InputLayout* p_IL = new InputLayout();

		p_IL->Build();

		m_inputLayouts[PsoType::ParticleDraw] = p_IL;
		m_pipelineStateBuilders[PsoType::ParticleDraw].Build(m_shaders[PsoType::ParticleDraw], p_IL, mp_rootSignDrawParticle, false, D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT);
	}

	void PsoManager::BuildParticleUpdate()
	{
		m_shaders[PsoType::ParticleUpdate] = Shader::Create("ParticleUpdate", PATH_SHADER_PARTICLE_UPDATE, true);

		InputLayout* p_IL = nullptr;
		m_inputLayouts[PsoType::ParticleUpdate] = p_IL;

		m_pipelineStateBuilders[PsoType::ParticleUpdate].BuildCompute(m_shaders[PsoType::ParticleUpdate], mp_rootSignComputeParticleUpdate);
	}

	void PsoManager::BuildParticleSpawn()
	{
		m_shaders[PsoType::ParticleSpawn] = Shader::Create("ParticleSpawn", PATH_SHADER_PARTICLE_SPAWN, true);

		InputLayout* p_IL = nullptr;
		m_inputLayouts[PsoType::ParticleSpawn] = p_IL;

		m_pipelineStateBuilders[PsoType::ParticleSpawn].BuildCompute(m_shaders[PsoType::ParticleSpawn], mp_rootSignComputeParticleSpawn);
	}

	ID3D12PipelineState* PsoManager::GetPSO(PsoType type)
	{
		auto it = m_pipelineStateBuilders.find(type);

		if (it == m_pipelineStateBuilders.end())
			return nullptr;

		return it->second.mp_PSO;
	}
}

