#pragma once

struct IDXGISwapChain;
struct ID3D12Resource;

namespace nam
{
	class RenderManager;

	class SwapChainManager
	{
	public:
		static const int SwapChainBufferCount = 2;

	private:
		RenderManager* mp_owner = nullptr;
		IDXGISwapChain* mp_swapChain = nullptr;
		ID3D12Resource* mp_swapChainBuffer[SwapChainBufferCount];

		DXGI_FORMAT m_backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

		int m_currentBackBuffer = 0;
	public:
		SwapChainManager();
		~SwapChainManager();

		void Init();
		void SwapBackBuffer();

		void ResetResources();
		void Resize();

		ID3D12Resource* CurrentBackBuffer() const;
		const int GetCurrentBackBufferNumber() const;
		D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;

		const DXGI_FORMAT GetBackBufferFormat() const;

		IDXGISwapChain*& GetSwapChain();

		ID3D12Resource*& GetSwapChainBuffer(size_t index);

	};
}