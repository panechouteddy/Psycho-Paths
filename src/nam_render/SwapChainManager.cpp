#include "pch.h"
#include "SwapChainManager.h"
#include "RenderManager.h"
#include "CommandManager.h"
#include "../nam_engine/App.h"

namespace nam
{
	SwapChainManager::SwapChainManager()
	{
		mp_owner = RENDER_MANAGER;
	}

	SwapChainManager::~SwapChainManager()
	{
		for (size_t i = 0; i < SwapChainBufferCount; i++)
		{
			mp_swapChainBuffer[i]->Release();
		}

		mp_swapChain->Release();
	}

	void SwapChainManager::Init()
	{
		if (mp_swapChain != nullptr)
		{
			mp_swapChain->Release();
			mp_swapChain = nullptr;
		}

		Window* p_window = mp_owner->GetWindow();

		DXGI_SWAP_CHAIN_DESC sd;
		sd.BufferDesc.Width = p_window->m_clientWidth;
		sd.BufferDesc.Height = p_window->m_clientHeight;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferDesc.Format = m_backBufferFormat;
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		sd.SampleDesc.Count = mp_owner->m_4xMsaaState ? 4 : 1;
		sd.SampleDesc.Quality = mp_owner->m_4xMsaaState ? (mp_owner->m_4xMsaaQuality - 1) : 0;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = SwapChainBufferCount;
		sd.OutputWindow = p_window->m_hwnd;
		sd.Windowed = true;
		sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		// Note: Swap chain uses queue to perform flush.
		ThrowIfFailed(mp_owner->GetDXGIFactory()->CreateSwapChain(
			mp_owner->GetCommandManager()->GetCommandQueue(),
			&sd,
			&mp_swapChain));

	}

	void SwapChainManager::SwapBackBuffer()
	{
		ThrowIfFailed(mp_swapChain->Present(1, 0));
		m_currentBackBuffer = (m_currentBackBuffer + 1) % SwapChainBufferCount;
	}

	void SwapChainManager::Resize()
	{
		Window* p_window = mp_owner->GetWindow();

		ThrowIfFailed(mp_swapChain->ResizeBuffers(
			SwapChainBufferCount,
			p_window->m_clientWidth, p_window->m_clientHeight,
			m_backBufferFormat,
			DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

		m_currentBackBuffer = 0;
	}

	void SwapChainManager::ResetResources()
	{
		for (int i = 0; i < SwapChainBufferCount; ++i)
		{
			ID3D12Resource*& current = mp_swapChainBuffer[i];

			if (current != nullptr)
			{
				current->Release();
				current = nullptr;
			}
		}
	}

	ID3D12Resource* SwapChainManager::CurrentBackBuffer() const
	{
		return mp_swapChainBuffer[m_currentBackBuffer];
	}

	const int SwapChainManager::GetCurrentBackBufferNumber() const
	{
		return m_currentBackBuffer;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE SwapChainManager::CurrentBackBufferView() const
	{
		DescriptorHeapsWrapper* p_descHeapWrap = mp_owner->GetDescriptorHeapsWrapper();

		return CD3DX12_CPU_DESCRIPTOR_HANDLE(
			p_descHeapWrap->m_rtvHeap->GetCPUDescriptorHandleForHeapStart(),
			m_currentBackBuffer,
			p_descHeapWrap->m_rtvDescriptorSize);
	}
	const DXGI_FORMAT SwapChainManager::GetBackBufferFormat() const
	{
		return m_backBufferFormat;
	}
	IDXGISwapChain*& SwapChainManager::GetSwapChain()
	{
		return mp_swapChain;
	}
	ID3D12Resource*& SwapChainManager::GetSwapChainBuffer(size_t index)
	{
		return mp_swapChainBuffer[index];
	}
}