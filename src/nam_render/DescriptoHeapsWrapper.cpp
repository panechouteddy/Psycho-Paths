#include "pch.h"
#include "DescriptoHeapsWrapper.h"
#include "RenderManager.h"
#include "SwapChainManager.h"
#include "CommandManager.h"

namespace nam
{
	DescriptorHeapsWrapper::~DescriptorHeapsWrapper()
	{
		UnInit();
	}

	void DescriptorHeapsWrapper::UnInit()
	{
		if (m_rtvHeap)
		{
			m_rtvHeap->Release();
			m_rtvHeap = nullptr;
		}

		if (m_dsvHeap)
		{
			m_dsvHeap->Release();
			m_dsvHeap = nullptr;
		}

		if (m_globalHeap)
		{
			m_globalHeap->Release();
			m_globalHeap = nullptr;
		}
	}

	void DescriptorHeapsWrapper::CreateRTVandDescriptorHeaps()
	{
		ID3D12Device* p_device = RENDER_MANAGER->GetDevice();

		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
		rtvHeapDesc.NumDescriptors = RENDER_MANAGER->GetSwapChainManager()->SwapChainBufferCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvHeapDesc.NodeMask = 0;
		ThrowIfFailed(p_device->CreateDescriptorHeap(
			&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));

		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		dsvHeapDesc.NodeMask = 0;
		ThrowIfFailed(p_device->CreateDescriptorHeap(
			&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap)));

		D3D12_DESCRIPTOR_HEAP_DESC globalHeapDesc;
		globalHeapDesc.NumDescriptors = MAX_DESCRIPTORS;
		globalHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		globalHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		globalHeapDesc.NodeMask = 0;
		ThrowIfFailed(p_device->CreateDescriptorHeap(
			&globalHeapDesc, IID_PPV_ARGS(&m_globalHeap)));
	}
	void DescriptorHeapsWrapper::SetDescriptorsHeaps()
	{
		ID3D12DescriptorHeap* descriptorHeaps[] = { m_globalHeap };
		RENDER_MANAGER->GetCommandManager()->GetCommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	}
	void DescriptorHeapsWrapper::SetDescriptorSizes()
	{
		ID3D12Device* p_device = RENDER_MANAGER->GetDevice();

		m_rtvDescriptorSize = p_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		m_dsvDescriptorSize = p_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		m_cbvSrvUavDescriptorSize = p_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	ID3D12DescriptorHeap* DescriptorHeapsWrapper::GetRTV_Heap() const
	{
		return m_rtvHeap;
	}

	ID3D12DescriptorHeap* DescriptorHeapsWrapper::GetDSV_Heap() const
	{
		return m_dsvHeap;
	}

	ID3D12DescriptorHeap* DescriptorHeapsWrapper::GetGlobal_Heap() const
	{
		return m_globalHeap;
	}
	D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeapsWrapper::GetUAVHandle(UINT offset) const
	{
		return CD3DX12_GPU_DESCRIPTOR_HANDLE(
			m_globalHeap->GetGPUDescriptorHandleForHeapStart(),
			offset,
			m_cbvSrvUavDescriptorSize
		);
	}
	D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeapsWrapper::GetSRVHandle(UINT offset) const
	{
		return CD3DX12_GPU_DESCRIPTOR_HANDLE(
			m_globalHeap->GetGPUDescriptorHandleForHeapStart(),
			offset,
			m_cbvSrvUavDescriptorSize
		);
	}
}