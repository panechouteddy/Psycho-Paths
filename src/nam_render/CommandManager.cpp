#include "pch.h"
#include "CommandManager.h"
#include "RenderManager.h"

namespace nam
{
	CommandManager::CommandManager()
	{
		mp_owner = RENDER_MANAGER;
	}

	CommandManager::~CommandManager()
	{
		m_commandList->Release();
		m_commandList = nullptr;

		m_directCmdListAlloc->Release();
		m_directCmdListAlloc = nullptr;

		m_commandQueue->Release();
		m_commandQueue = nullptr;
	}

	void CommandManager::Init()
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

		ID3D12Device* p_device = mp_owner->GetDevice();

		ThrowIfFailed(p_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));

		ThrowIfFailed(p_device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&m_directCmdListAlloc)));

		ThrowIfFailed(p_device->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			m_directCmdListAlloc,
			nullptr,
			IID_PPV_ARGS(&m_commandList)));

		m_commandList->Close();
	}

	void CommandManager::FlushCommandQueue()
	{
		ID3D12Fence* p_fence = mp_owner->GetFence();
		UINT64& currentFence = mp_owner->GetCurrentFence();
		currentFence++;

		ThrowIfFailed(m_commandQueue->Signal(p_fence, currentFence));

		// Wait until the GPU has completed commands up to this fence point.
		if (p_fence->GetCompletedValue() < currentFence)
		{
			HANDLE eventHandle = CreateEventEx(nullptr, L"Event Fence", false, EVENT_ALL_ACCESS);

			// Fire event when GPU hits current fence.  
			ThrowIfFailed(p_fence->SetEventOnCompletion(currentFence, eventHandle));

			if (eventHandle == 0)
				return;

			// Wait until the GPU hits current fence event is fired.
			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}
	}

	void CommandManager::ResetCommandList(ID3D12PipelineState* p_pso)
	{
		FlushCommandQueue();
		ThrowIfFailed(m_directCmdListAlloc->Reset());
		ThrowIfFailed(m_commandList->Reset(m_directCmdListAlloc, p_pso));
	}

	void CommandManager::CloseCommandList()
	{
		ThrowIfFailed(m_commandList->Close());
	}

	void CommandManager::ExecuteCommandList()
	{
		ID3D12CommandList* cmdsLists[] = { m_commandList };
		m_commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
	}
}