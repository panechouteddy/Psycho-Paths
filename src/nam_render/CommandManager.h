#pragma once
#include "Common/d3dUtil.h"

struct ID3D12CommandQueue;
struct ID3D12CommandAllocator;
struct ID3D12GraphicsCommandList;

namespace nam
{
	class RenderManager;

	class CommandManager
	{
		RenderManager* mp_owner = nullptr;

		ID3D12CommandQueue* m_commandQueue = nullptr;
		ID3D12CommandAllocator* m_directCmdListAlloc = nullptr;
		ID3D12GraphicsCommandList* m_commandList = nullptr;
	public:
		CommandManager();
		~CommandManager();

		void Init();
		void FlushCommandQueue();

		void ResetCommandList(ID3D12PipelineState* p_pso);
		void CloseCommandList();
		void ExecuteCommandList();

		ID3D12CommandAllocator* GetCommandAllocator() const { return m_directCmdListAlloc; }
		ID3D12GraphicsCommandList* GetCommandList() const { return m_commandList; }
		ID3D12CommandQueue* GetCommandQueue() const { return m_commandQueue; }
	};
}

