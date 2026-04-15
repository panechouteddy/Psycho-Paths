#pragma once
#include "DataShader.h"
#include "DescriptoHeapsWrapper.h"
#include "Common/d3dUtil.h"

#include "RootSignature.h"
#include "InputLayout.h"
#include "Shader.h"

#include "PipelineStateBuilder.h"
#include "PsoManager.h"

#include "ConstantBuffersDatas.h"
#include "TextureManager.h"
#include "LightManager.h"
#include "RenderItemManager.h"
#include "MaterialManager.h"

#include "DebugLog.h"

#include <Benchmark.h>
 
#define RENDER_MANAGER RenderManager::Get()

struct ID3D12Device;
struct ID3D12Fence;
struct IDXGIFactory4;
struct ID3D12RootSignature;
struct ID3D12Resource;
struct ID3D12PipelineState;

template<typename T>
class UploadBuffer;

namespace nam
{
	struct Window;
	struct ARenderItem;

	class ParticleManager;

	class SwapChainManager;
	class CommandManager;

	class RenderManager
	{
	public:
		bool m_4xMsaaState = false;    // 4X MSAA enabled
		UINT m_4xMsaaQuality = 0;      // quality level of 4X MSAA
		D3D_DRIVER_TYPE m_d3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
		DXGI_FORMAT m_depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	private:
		RenderManager(Window* p_window);

		static RenderManager* msp_instance;

		Window* mp_window = nullptr;

		SwapChainManager* mp_swapChainManager = nullptr;
		CommandManager* mp_commandManager = nullptr;
		static ParticleManager* msp_particleManager;

		DescriptorHeapsWrapper m_descHeapsWrap;

		ID3D12Device* mp_device = nullptr;

		ID3D12Fence* mp_fence = nullptr;
		UINT64 m_currentFence = 0;

		IDXGIFactory4* mp_dxgiFactory = nullptr;

		D3D12_VIEWPORT m_screenViewport = {};
		D3D12_RECT m_scissorRect = {};

		PsoManager m_psoManager;

		ID3D12Resource* mp_depthStencilBuffer = nullptr;

		//Cbuffers
		ConstantBuffersDatas m_cbuffers;

		UnMap<PsoType, Vector<ARenderItem*>> m_renderItemsToAdd;
		UnMap<PsoType, Vector<ARenderItem*>> m_renderItemsToDraw;

		TextureManager m_textureManager;
		LightManager m_lightManager;
		RenderItemManager m_renderItemManager;
		MaterialManager m_materialManager;

		UINT m_activeParticles = 0;

		bool m_firstDraw = true;
	public:
		static RenderManager* Get();
		static void Init(Window* p_window = nullptr);
		static void UnInit();

		~RenderManager();

		bool InitDirectX3D();

		//Build 2D, Textures
		void BuildMinimal();

		//Build 3D, Particles and Lights
		void BuildOthers();

		void EndInit(); //To use after Build All or a Load Texture to begin the main loop

		void OnResize();
		
		void CleanMeshes();
		void AddToDraw(ARenderItem* p_rItem);
		void ToDraw(ARenderItem* p_rItem);

		void SetActiveParticles(UINT count);

		void BeginFrame();
		void UpdateCompute();
		void Draw();
		void EndFirstFrame();
		void EndFrame();

		void LoadTexture(std::wstring path, size uniqueTag, bool usingTextureFolder);

		template<typename T>
		void SetConstantBuffer(size_t index, UploadBuffer<T>* buffer);

	public: // Getters
		SwapChainManager* GetSwapChainManager();
		CommandManager* GetCommandManager();
		ParticleManager* GetParticleManager();
		DescriptorHeapsWrapper* GetDescriptorHeapsWrapper();

		ID3D12Device* GetDevice() const;
		ID3D12Fence* GetFence() const;
		UINT64& GetCurrentFence();

		IDXGIFactory4* GetDXGIFactory() const;

		const D3D12_VIEWPORT& GetScreenViewPort() const;
		const D3D12_RECT& GetScissorRect() const;

		ConstantBuffersDatas* GetConstantBuffersDatas();

		D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;

		PsoManager* GetPsoManager();
		TextureManager* GetTextureManager();

		Window* GetWindow() const;
		LightManager& GetLightManager();
		RenderItemManager& GetRenderItemManager();
		MaterialManager& GetMaterialManager();

		const bool& IsFirstDraw() const;

	private:
		void CreateDevice();

		void HandleX4MSAAQualitySupport();
		void CreateRenderTargetView();
		void CreateDepthStencilBuffersViews();

		void SetupParticleUAV();
		void SetupParticleSRV();

		void ClearRenderTarget();
		void SetNewRenderTarget();

		void SetViewPort();
		void SetScissorRect();

		void RSSetViewPortAndScissorRect();
		void SetGraphicsRootSignature(RootSignatureName rootSignatureName);
		void SetComputeRootSignature(RootSignatureName rootSignatureName);

		//Draw Loop
		void Render3D(ID3D12GraphicsCommandList* p_commandList);
		void Render2D(ID3D12GraphicsCommandList* p_commandList);
		void RenderParticles(ID3D12GraphicsCommandList* p_commandList);

		friend class PsoManager;
		friend class ParticleManager;
	};

}

#include "RenderManager.inl"
