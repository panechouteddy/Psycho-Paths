#include "pch.h"
#include "RenderManager.h"
#include "SwapchainManager.h"
#include "CommandManager.h"
#include "RenderWrapper.h"

#include "../nam_engine/App.h"

#include "Common/DDSTextureLoader.h"

#include "Mesh.h"
#include "Window.h"
#include "PipelineStateBuilder.h"

#include "ParticleManager.h"

#include "AnimationData.h"

namespace nam
{
	RenderManager* RenderManager::msp_instance = nullptr;
	ParticleManager* RenderManager::msp_particleManager = nullptr;

	RenderManager* RenderManager::Get()
	{
		if (msp_instance == nullptr)
		{
			FatalError("RenderManager needs to be init with a Window*");
		}

		return msp_instance;
	}
	void RenderManager::Init(Window* p_window)
	{
		if (p_window == nullptr && msp_instance == nullptr)
		{
			FatalError("RenderManager needs to be init with a Window*");
		}

		if (msp_instance == nullptr)
		{
			msp_instance = new RenderManager(p_window);
			msp_instance->mp_swapChainManager = new SwapChainManager();
			msp_instance->mp_commandManager = new CommandManager();
			msp_particleManager = new ParticleManager();
		}
	}

	void RenderManager::UnInit()
	{
		if (msp_instance)
		{
			msp_instance->mp_commandManager->FlushCommandQueue();
			delete msp_instance;
		}

		if (msp_particleManager)
		{
			delete msp_particleManager;
			msp_particleManager = nullptr;
		}
	}

	RenderManager::RenderManager(Window* p_window)
	{
		mp_window = p_window;
	}

	RenderManager::~RenderManager()
	{
		delete mp_commandManager;
		m_descHeapsWrap.UnInit();

		for (auto& [type, renderItems] : m_renderItemsToAdd)
		{
			for (ARenderItem*& p_rItem : renderItems)
			{
				if (p_rItem == nullptr)
					continue;

				delete p_rItem;

				p_rItem = nullptr;
			}
		}
		m_renderItemsToAdd.clear();

		for (auto& [type, renderItems] : m_renderItemsToDraw)
		{
			for (ARenderItem*& p_rItem : renderItems)
			{
				if (p_rItem == nullptr)
					continue;

				delete p_rItem;

				p_rItem = nullptr;
			}
		}

		m_renderItemsToDraw.clear();

		m_psoManager.UnInit();

		m_cbuffers.UnInit();
		m_textureManager.UnInit();

		delete mp_swapChainManager;

		mp_depthStencilBuffer->Release();

		mp_fence->Release();
		mp_dxgiFactory->Release();

		mp_device->Release();
	}

	bool RenderManager::InitDirectX3D()
	{
		CreateDevice();

		ThrowIfFailed(mp_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mp_fence)));

		m_descHeapsWrap.SetDescriptorSizes();
		HandleX4MSAAQualitySupport();

		mp_commandManager->Init();
		mp_swapChainManager->Init();

		m_descHeapsWrap.CreateRTVandDescriptorHeaps();
		CreateRenderTargetView();
		CreateDepthStencilBuffersViews();

		return true;
	}

	void RenderManager::CreateDevice()
	{
//#if defined(_DEBUG)
//		{
//			ComPtr<ID3D12Debug> debugController;
//			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
//			{
//				debugController->EnableDebugLayer();
//				debugController->Release();
//			}
//		}
//
//#endif

	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&mp_dxgiFactory)));

		// Try to create hardware device.
		HRESULT hardwareResult = D3D12CreateDevice(
			nullptr, // default adapter
			D3D_FEATURE_LEVEL_12_0,
			IID_PPV_ARGS(&mp_device));

		// Si pas de GPU 
		if (FAILED(hardwareResult))
		{
			ComPtr<IDXGIAdapter> pWarpAdapter;
			ThrowIfFailed(mp_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));
			ThrowIfFailed(D3D12CreateDevice(
				pWarpAdapter.Get(),
				D3D_FEATURE_LEVEL_12_0,
				IID_PPV_ARGS(&mp_device)));
		}
	}

	void RenderManager::HandleX4MSAAQualitySupport()
	{
		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels = {};
		msQualityLevels.Format = mp_swapChainManager->GetBackBufferFormat();
		msQualityLevels.SampleCount = 4;
		msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		msQualityLevels.NumQualityLevels = 0;
		ThrowIfFailed(mp_device->CheckFeatureSupport(
			D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
			&msQualityLevels,
			sizeof(msQualityLevels)));
		m_4xMsaaQuality = msQualityLevels.NumQualityLevels;
		assert(m_4xMsaaQuality > 0 && "Unexpected MSAA quality level.");
	}

	void RenderManager::CreateRenderTargetView()
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(m_descHeapsWrap.m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
		for (UINT i = 0; i < mp_swapChainManager->SwapChainBufferCount; i++)
		{
			ID3D12Resource*& buffer = mp_swapChainManager->GetSwapChainBuffer(i);
	
			ThrowIfFailed(mp_swapChainManager->GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&buffer)));
			buffer->SetName(L"BackBuffer Resource");

			mp_device->CreateRenderTargetView(buffer, nullptr, rtvHeapHandle);
			rtvHeapHandle.Offset(1, m_descHeapsWrap.m_rtvDescriptorSize);
		}
	}
	void RenderManager::CreateDepthStencilBuffersViews()
	{
		// Create the depth/stencil buffer and view.
		D3D12_RESOURCE_DESC depthStencilDesc = {};
		depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		depthStencilDesc.Alignment = 0;
		depthStencilDesc.Width = mp_window->m_clientWidth;
		depthStencilDesc.Height = mp_window->m_clientHeight;
		depthStencilDesc.DepthOrArraySize = 1;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.Format = m_depthStencilFormat;
		depthStencilDesc.SampleDesc.Count = m_4xMsaaState ? 4 : 1;
		depthStencilDesc.SampleDesc.Quality = m_4xMsaaState ? (m_4xMsaaQuality - 1) : 0;
		depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE optClear = {};
		optClear.Format = m_depthStencilFormat;
		optClear.DepthStencil.Depth = 1.0f;
		optClear.DepthStencil.Stencil = 0;

		D3D12_HEAP_PROPERTIES heap_props = {};
		heap_props.Type = D3D12_HEAP_TYPE_DEFAULT;
		heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heap_props.CreationNodeMask = 1;
		heap_props.VisibleNodeMask = 1;

		ThrowIfFailed(mp_device->CreateCommittedResource(
			&heap_props,
			D3D12_HEAP_FLAG_NONE,
			&depthStencilDesc,
			D3D12_RESOURCE_STATE_COMMON,
			&optClear,
			IID_PPV_ARGS(&mp_depthStencilBuffer)));

		// Create descriptor to mip level 0 of entire resource using the
		// format of the resource.
		mp_device->CreateDepthStencilView(
			mp_depthStencilBuffer,
			nullptr,
			DepthStencilView());

		mp_depthStencilBuffer->SetName(L"Depth Stencil Buffer");

		Render_Wrapper::CreateTransition(mp_depthStencilBuffer, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
	}

	void RenderManager::SetupParticleUAV()
	{
		ID3D12Device* p_device = GetDevice();
		DescriptorHeapsWrapper* p_heaps = GetDescriptorHeapsWrapper();

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = MAX_PARTICLES;
		uavDesc.Buffer.StructureByteStride = sizeof(CBufferParticleCompute);
		uavDesc.Buffer.CounterOffsetInBytes = 0;
		uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

		CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(
			p_heaps->m_globalHeap->GetCPUDescriptorHandleForHeapStart(),
			PARTICLE_UAV_SLOT,
			p_heaps->m_cbvSrvUavDescriptorSize
		);

		p_device->CreateUnorderedAccessView(
			msp_particleManager->GetParticleResource(),
			nullptr,
			&uavDesc,
			cpuHandle
		);
	}

	void RenderManager::SetupParticleSRV()
	{
		ID3D12Device* p_device = GetDevice();
		DescriptorHeapsWrapper* p_heaps = GetDescriptorHeapsWrapper();

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = MAX_PARTICLES;
		srvDesc.Buffer.StructureByteStride = sizeof(CBufferParticleCompute);
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

		CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(
			p_heaps->m_globalHeap->GetCPUDescriptorHandleForHeapStart(),
			PARTICLE_SRV_SLOT,
			p_heaps->m_cbvSrvUavDescriptorSize
		);

		p_device->CreateShaderResourceView(
			msp_particleManager->GetParticleResource(),
			&srvDesc,
			cpuHandle
		);
	}

	void RenderManager::SetViewPort()
	{
		m_screenViewport.TopLeftX = 0.0f;
		m_screenViewport.TopLeftY = 0.0f;
		m_screenViewport.Width = static_cast<float>(mp_window->m_clientWidth);
		m_screenViewport.Height = static_cast<float>(mp_window->m_clientHeight);
		m_screenViewport.MinDepth = 0.0f;
		m_screenViewport.MaxDepth = 1.0f;
	}
	void RenderManager::SetScissorRect()
	{
		m_scissorRect = { 0, 0, mp_window->m_clientWidth, mp_window->m_clientHeight };
	}
	void RenderManager::RSSetViewPortAndScissorRect()
	{
		ID3D12GraphicsCommandList* pCommandList = mp_commandManager->GetCommandList();

		pCommandList->RSSetViewports(1, &m_screenViewport);
		pCommandList->RSSetScissorRects(1, &m_scissorRect);
	}

	void RenderManager::ClearRenderTarget() // Clear Back Buffer & Depth Stenciel
	{
		ID3D12GraphicsCommandList* pCommandList = mp_commandManager->GetCommandList();

		auto currentBackBufferView = mp_swapChainManager->CurrentBackBufferView();
		pCommandList->ClearRenderTargetView(currentBackBufferView, Colors::Black, 0, nullptr);

		auto depthStencilView = DepthStencilView();
		pCommandList->ClearDepthStencilView(depthStencilView, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	}
	void RenderManager::SetNewRenderTarget()
	{
		auto currentBackBufferView = mp_swapChainManager->CurrentBackBufferView();
		auto depthStencilView = DepthStencilView();

		mp_commandManager->GetCommandList()->OMSetRenderTargets(1, &currentBackBufferView, true, &depthStencilView);
	}

	void RenderManager::SetGraphicsRootSignature(RootSignatureName rootSignatureName)
	{
		if (m_psoManager.IsRootSignChangeUseless(rootSignatureName))
			return;

		mp_commandManager->GetCommandList()->SetGraphicsRootSignature(m_psoManager.GetRootSignature(rootSignatureName)->mp_get);
	}
	
	void RenderManager::SetComputeRootSignature(RootSignatureName rootSignatureName)
	{
		if (m_psoManager.IsRootSignChangeUseless(rootSignatureName))
			return;

		mp_commandManager->GetCommandList()->SetComputeRootSignature(m_psoManager.GetRootSignature(rootSignatureName)->mp_get);
	}

	void RenderManager::BuildMinimal()
	{
		mp_commandManager->ResetCommandList(nullptr);

		m_cbuffers.CreateMinimal();

		m_psoManager.BuildMinimal();
		m_textureManager.CreateTexture(L"../../src/nam_render/Splash Screens/prototype.dds", _SplashScreen, false);

		mp_commandManager->CloseCommandList();
		mp_commandManager->ExecuteCommandList();

		mp_commandManager->FlushCommandQueue();
	}

	void RenderManager::BuildOthers()
	{
		m_cbuffers.CreateOthers();

		mp_commandManager->ResetCommandList(nullptr);

		SetupParticleUAV();
		SetupParticleSRV();

		m_psoManager.BuildOthers();

		msp_particleManager->Init(mp_device, mp_commandManager->GetCommandList());
		msp_particleManager->CreateViews(
			mp_device,
			m_descHeapsWrap.m_globalHeap,
			PARTICLE_SRV_SLOT,
			PARTICLE_UAV_SLOT,
			PARTICLE_FREELIST_SLOT,
			PARTICLE_FREECOUNT_SLOT,
			PARTICLE_SPAWN_SLOT,
			m_descHeapsWrap.m_cbvSrvUavDescriptorSize
		);
	}

	void RenderManager::EndInit()
	{
		mp_commandManager->CloseCommandList();
		mp_commandManager->ExecuteCommandList();

		mp_commandManager->FlushCommandQueue();
	}

	void RenderManager::LoadTexture(std::wstring path, size uniqueTag, bool usingTextureFolder)
	{
		m_textureManager.CreateTexture(path, uniqueTag, usingTextureFolder);
	}

	void RenderManager::OnResize()
	{
		_ASSERT(mp_device);

		// Flush before changing any resources.
		mp_commandManager->ResetCommandList(nullptr);

		mp_swapChainManager->ResetResources();
		mp_swapChainManager->Resize();

		CreateRenderTargetView();
		CreateDepthStencilBuffersViews();

		// Update the viewport transform to cover the client area.
		SetViewPort();
		SetScissorRect();

		mp_commandManager->CloseCommandList();
		mp_commandManager->ExecuteCommandList();
	}

	void RenderManager::CleanMeshes()
	{
		for (auto& [type, meshes] : m_renderItemsToAdd)
		{
			for (size_t i = 0; i < meshes.size(); i++)
			{
				ToDraw(meshes[i]);
			}
		}

		m_renderItemsToAdd.clear();
	}

	void RenderManager::AddToDraw(ARenderItem* p_rItem)
	{
		m_renderItemsToAdd[p_rItem->GetPsoToUse()].push_back(p_rItem);
	}

	void RenderManager::ToDraw(ARenderItem* p_rItem)
	{
		p_rItem->InitEmptyBuffers();
		p_rItem->UpdateBuffers();

		m_renderItemsToDraw[p_rItem->GetPsoToUse()].push_back(p_rItem);
	}

	void RenderManager::SetActiveParticles(UINT count)
	{
		m_activeParticles = count;
	}

	void RenderManager::BeginFrame()
	{
		ID3D12GraphicsCommandList* p_commandList = mp_commandManager->GetCommandList();

		mp_commandManager->ResetCommandList(nullptr);
		RSSetViewPortAndScissorRect();

		Render_Wrapper::CreateTransition(mp_swapChainManager->CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		ClearRenderTarget();
		CleanMeshes();

		SetNewRenderTarget();

		m_descHeapsWrap.SetDescriptorsHeaps();
	}

	void RenderManager::UpdateCompute()
	{
		if (m_firstDraw)
			return;

		ID3D12GraphicsCommandList* cmdList = mp_commandManager->GetCommandList();
		msp_particleManager->FlushSpawns(cmdList);
		msp_particleManager->Update(cmdList);
	}

	void RenderManager::Draw()
	{
		ID3D12GraphicsCommandList* p_commandList = mp_commandManager->GetCommandList();

		if (m_firstDraw)
		{
			p_commandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			Render2D(p_commandList);
			return;
		}

		p_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		RenderParticles(p_commandList);

		p_commandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Render3D(p_commandList);
		Render2D(p_commandList);
	}
	void RenderManager::EndFirstFrame()
	{
		Render_Wrapper::CreateTransition(mp_swapChainManager->CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

		mp_commandManager->CloseCommandList();
		mp_commandManager->ExecuteCommandList();
		mp_commandManager->FlushCommandQueue();

		mp_swapChainManager->SwapBackBuffer();

		m_renderItemsToDraw.clear();

		if (m_firstDraw)
			m_firstDraw = false;
	}
	void RenderManager::EndFrame()
	{
		Render_Wrapper::CreateTransition(mp_swapChainManager->CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

		mp_commandManager->CloseCommandList();
		mp_commandManager->ExecuteCommandList();
		mp_swapChainManager->SwapBackBuffer();
		mp_commandManager->FlushCommandQueue();

		m_renderItemsToDraw.clear();
	}

	ID3D12Fence* RenderManager::GetFence() const
	{
		return mp_fence;
	}
	UINT64& RenderManager::GetCurrentFence()
	{
		return m_currentFence;
	}

	IDXGIFactory4* RenderManager::GetDXGIFactory() const
	{
		return mp_dxgiFactory;
	}
	const D3D12_VIEWPORT& RenderManager::GetScreenViewPort() const
	{
		return m_screenViewport;
	}
	const D3D12_RECT& RenderManager::GetScissorRect() const
	{
		return m_scissorRect;
	}

	ConstantBuffersDatas* RenderManager::GetConstantBuffersDatas() 
	{
		return &m_cbuffers;
	}

	SwapChainManager* RenderManager::GetSwapChainManager()
	{
		return mp_swapChainManager;
	}
	CommandManager* RenderManager::GetCommandManager()
	{
		return mp_commandManager;
	}
	ParticleManager* RenderManager::GetParticleManager()
	{
		return msp_particleManager;
	}
	DescriptorHeapsWrapper* RenderManager::GetDescriptorHeapsWrapper()
	{
		return &m_descHeapsWrap;
	}

	ID3D12Device* RenderManager::GetDevice() const
	{
		return mp_device;
	}

	TextureManager* RenderManager::GetTextureManager()
	{
		return &m_textureManager;
	}

	Window* RenderManager::GetWindow() const
	{
		return mp_window;
	}

	LightManager& RenderManager::GetLightManager()
	{
		return m_lightManager;
	}

	RenderItemManager& RenderManager::GetRenderItemManager()
	{
		return m_renderItemManager;
	}

	MaterialManager& RenderManager::GetMaterialManager()
	{
		return m_materialManager;
	}

	const bool& RenderManager::IsFirstDraw() const
	{
		return m_firstDraw;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE RenderManager::DepthStencilView() const
	{
		return m_descHeapsWrap.m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
	}

	PsoManager* RenderManager::GetPsoManager()
	{
		return &m_psoManager;
	}

	void RenderManager::Render3D(ID3D12GraphicsCommandList* p_commandList)
	{
		Vector<ARenderItem*>& meshesToDraw = m_renderItemsToDraw[PsoType::_Render3D];

		if (meshesToDraw.empty())
			return;

		p_commandList->SetPipelineState(m_psoManager.GetPSO(PsoType::_Render3D));
		SetGraphicsRootSignature(RootSignatureName::_Root3D);

		p_commandList->SetGraphicsRootConstantBufferView(1, m_cbuffers.GetGPUVirtualAdressOf(CBufferType::_ViewProj));
		p_commandList->SetGraphicsRootConstantBufferView(2, m_cbuffers.GetGPUVirtualAdressOf(CBufferType::_Lights));

		const int alignValue = 256;

		for (size_t i = 0; i < meshesToDraw.size(); i++)
		{
			ARenderItem* p_mesh = meshesToDraw[i];
			p_mesh->BindBuffers();

			CBUfferSkinning skinBuffer = {};

			//if (MeshAnimationModel* p_anim = p_mesh->GetAnimModel())
			//{
			//	p_anim->m_clipName = "Salute";
			//	p_anim->UpdateAnimation(CHRONO.GetScaledDeltaTime());

			//	Vector<DirectX::XMFLOAT4X4>& finalTransforms = p_anim->m_finalTransforms;

			//	for (size_t i = 0; i < finalTransforms.size(); ++i) {
			//	/*	skinBuffer.m_boneTransforms[i] = finalTransforms[i];*/
			//		DirectX::XMStoreFloat4x4(&skinBuffer.m_boneTransforms[i], DirectX::XMMatrixIdentity());
			//	}
			//	// Remplir les os restants avec l'identité
			//	for (size_t i = finalTransforms.size(); i < 256; ++i) {
			//		DirectX::XMStoreFloat4x4(&skinBuffer.m_boneTransforms[i], DirectX::XMMatrixIdentity());
			//	}
			//}
		
			m_cbuffers.mp_skinningCB->CopyData(0, skinBuffer);
			p_commandList->SetGraphicsRootConstantBufferView(4, m_cbuffers.GetGPUVirtualAdressOf(CBufferType::_Skinning));

			UINT indicesCount = static_cast<UINT>(p_mesh->m_indices.size());
			UINT64 cbvOffset = i * alignValue; // aligned
			UINT64 matOffset = p_mesh->GetMaterialTag() * alignValue;

			//Texture
			UINT offset = p_mesh->GetTextureTag();
			CD3DX12_GPU_DESCRIPTOR_HANDLE handleTextureIndex(
				m_descHeapsWrap.m_globalHeap->GetGPUDescriptorHandleForHeapStart(),
				offset,
				m_descHeapsWrap.m_cbvSrvUavDescriptorSize
			);

			p_commandList->SetGraphicsRootDescriptorTable(5, handleTextureIndex);
			p_commandList->SetGraphicsRootConstantBufferView(3, m_cbuffers.GetGPUVirtualAdressOf(CBufferType::_Material) + matOffset);
			p_commandList->SetGraphicsRootConstantBufferView(0, m_cbuffers.GetGPUVirtualAdressOf(CBufferType::_Obj3D) + cbvOffset);
			p_commandList->DrawIndexedInstanced(indicesCount, 1, 0, 0, 0);
		}
	}

	void RenderManager::Render2D(ID3D12GraphicsCommandList* p_commandList)
	{
		Vector<ARenderItem*>& spritesToDraw = m_renderItemsToDraw[PsoType::_Render2D];

		if (spritesToDraw.empty())
			return;

		p_commandList->SetPipelineState(m_psoManager.GetPSO(PsoType::_Render2D));
		SetGraphicsRootSignature(RootSignatureName::_Root2D);
		p_commandList->SetGraphicsRootConstantBufferView(1, m_cbuffers.GetGPUVirtualAdressOf(CBufferType::_Color2DOrtho));

		for (size_t i = 0; i < spritesToDraw.size(); i++)
		{
			ARenderItem* p_sprite = spritesToDraw[i];
			p_sprite->BindBuffers();

			UINT indicesCount = static_cast<UINT>(p_sprite->m_indices.size());
			UINT64 cbvOffset = i * 256; // aligned

			//Texture
			UINT offset = p_sprite->GetTextureTag();
			CD3DX12_GPU_DESCRIPTOR_HANDLE handleTextureIndex(
				m_descHeapsWrap.m_globalHeap->GetGPUDescriptorHandleForHeapStart(),
				offset,
				m_descHeapsWrap.m_cbvSrvUavDescriptorSize
			);

			p_commandList->SetGraphicsRootDescriptorTable(2, handleTextureIndex);

			p_commandList->SetGraphicsRootConstantBufferView(0, m_cbuffers.GetGPUVirtualAdressOf(CBufferType::_Sprite2D) + cbvOffset);
			p_commandList->DrawIndexedInstanced(indicesCount, 1, 0, 0, 0);
		}
	}

	void RenderManager::RenderParticles(ID3D12GraphicsCommandList* p_commandList)
	{
		auto uavBarrier = CD3DX12_RESOURCE_BARRIER::UAV(msp_particleManager->GetParticleResource());
		p_commandList->ResourceBarrier(1, &uavBarrier);

		auto* rootSig = m_psoManager.GetRootSignature(RootSignatureName::_RootDrawParticle)->mp_get;
		p_commandList->SetGraphicsRootSignature(rootSig);

		p_commandList->SetPipelineState(m_psoManager.GetPSO(PsoType::ParticleDraw));

		p_commandList->SetGraphicsRootConstantBufferView(0, m_cbuffers.GetGPUVirtualAdressOf(CBufferType::_ViewProj));

		CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle(
			m_descHeapsWrap.m_globalHeap->GetGPUDescriptorHandleForHeapStart(),
			PARTICLE_SRV_SLOT,
			m_descHeapsWrap.m_cbvSrvUavDescriptorSize
		);
		p_commandList->SetGraphicsRootDescriptorTable(1, srvHandle);

		p_commandList->DrawInstanced(1, msp_particleManager->GetMaxParticles(), 0, 0);
	}

}