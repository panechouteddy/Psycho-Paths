#pragma once
#include "PipelineStateBuilder.h"
#include "Shader.h"
#include "InputLayout.h"
#include "RootSignature.h"

#include "ContainerHash.h"
#include "RootSignatureName.h"

namespace nam
{
	class PsoManager
	{
		RootSignature* mp_rootSign3D = nullptr;
		RootSignature* mp_rootSign2D = nullptr;
		RootSignature* mp_rootSignDrawParticle = nullptr;
		RootSignature* mp_rootSignComputeParticleUpdate = nullptr;
		RootSignature* mp_rootSignComputeParticleSpawn = nullptr;

		UnMap<PsoType, Shader*> m_shaders;
		UnMap<PsoType, InputLayout*> m_inputLayouts;
		UnMap<PsoType, PipelineStateBuilder> m_pipelineStateBuilders;

		bool m_rootSignFirstBinded = false; // Set to True when the Render use it for the first time
		RootSignatureName m_currentRootSign = RootSignatureName::_None;

		void BuildRootSignatures();
		void BuildOthersRootSignatures();
	public:
		~PsoManager();

		void UnInit();

		void BuildMinimal();
		void BuildOthers();

		void Build3DTexture();
		void Build2DTexture();
		void BuildParticleDraw();
		void BuildParticleUpdate();
		void BuildParticleSpawn();

		bool IsRootSignChangeUseless(RootSignatureName rootSignatureType);
		RootSignature* GetRootSignature(RootSignatureName rootSignatureType);
		ID3D12PipelineState* GetPSO(PsoType type);
	};
}

