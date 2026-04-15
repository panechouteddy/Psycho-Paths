#include "pch.h"
#include "PipelineStateBuilder.h"
#include "RenderManager.h"
#include "SwapChainManager.h"

#include "Shader.h"
#include "RootSignature.h"
#include "InputLayout.h"

namespace nam
{
	PipelineStateBuilder::~PipelineStateBuilder()
	{
		UnInit();
	}

	void PipelineStateBuilder::UnInit()
	{
		if (mp_PSO)
		{
			mp_PSO->Release();
			mp_PSO = nullptr;
		}
	}

	void PipelineStateBuilder::Build(Shader* p_shader, InputLayout* p_inputLayout, RootSignature* p_rootSignature, bool for2D, D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType)
	{
		m_for2D = for2D;

		mp_shader = p_shader;
		mp_inputLayout = p_inputLayout;
		mp_rootSignature = p_rootSignature;

		BuildPSO(topologyType);
	}

	void PipelineStateBuilder::BuildCompute(Shader* p_shader, RootSignature* p_rootSignature)
	{
		mp_shader = p_shader;
		mp_rootSignature = p_rootSignature;

		RenderManager* p_renderManager = RENDER_MANAGER;

		D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.pRootSignature = mp_rootSignature->mp_get;
		psoDesc.CS =
		{
			reinterpret_cast<BYTE*>(mp_shader->mp_csByteCode->GetBufferPointer()),
			mp_shader->mp_csByteCode->GetBufferSize()
		};

		ThrowIfFailed(p_renderManager->GetDevice()->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&mp_PSO)));
	}

	void PipelineStateBuilder::BuildPSO(D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType)
	{
		RenderManager* p_renderManager = RENDER_MANAGER;

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
		ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

		psoDesc.InputLayout = mp_inputLayout->GetDesc();
		psoDesc.pRootSignature = mp_rootSignature->mp_get;

		psoDesc.VS =
		{
			reinterpret_cast<BYTE*>(mp_shader->mp_vsByteCode->GetBufferPointer()),
			mp_shader->mp_vsByteCode->GetBufferSize()
		};
		psoDesc.PS =
		{
			reinterpret_cast<BYTE*>(mp_shader->mp_psByteCode->GetBufferPointer()),
			mp_shader->mp_psByteCode->GetBufferSize()
		};

		D3D12_RASTERIZER_DESC rastDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

		if (m_for2D)
			rastDesc.CullMode = D3D12_CULL_MODE_NONE;

		psoDesc.RasterizerState = rastDesc;
	

		D3D12_BLEND_DESC blendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

		if (m_for2D)
		{	
			blendDesc.RenderTarget[0].BlendEnable = TRUE;
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;

			blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
			blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
			blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;

			blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		}

		psoDesc.BlendState = blendDesc;

		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = topologyType;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = p_renderManager->GetSwapChainManager()->GetBackBufferFormat();
		psoDesc.SampleDesc.Count = p_renderManager->m_4xMsaaState ? 4 : 1;
		psoDesc.SampleDesc.Quality = p_renderManager->m_4xMsaaState ? (p_renderManager->m_4xMsaaQuality - 1) : 0;
	
		if (m_for2D)
		{
			D3D12_DEPTH_STENCIL_DESC depthDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
			depthDesc.DepthEnable = FALSE;
			depthDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
			depthDesc.StencilEnable = FALSE;

			psoDesc.DepthStencilState = depthDesc;
			psoDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;
		}
		else
		{
			psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
			psoDesc.DSVFormat = p_renderManager->m_depthStencilFormat;
		}

		ThrowIfFailed(p_renderManager->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mp_PSO)));
	}
}