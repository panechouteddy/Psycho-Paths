#pragma once
#include "RenderManager.h"
#include "Common/d3dx12.h"
#include "CommandManager.h"

namespace nam
{
	struct Render_Wrapper
	{
		static void CreateTransition(ID3D12Resource* pResource,
			D3D12_RESOURCE_STATES stateBefore,
			D3D12_RESOURCE_STATES stateAfter,
			UINT subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
			D3D12_RESOURCE_BARRIER_FLAGS flags = D3D12_RESOURCE_BARRIER_FLAG_NONE)
		{
			auto transition = CD3DX12_RESOURCE_BARRIER::Transition(pResource, stateBefore, stateAfter, subresource, flags);
			RENDER_MANAGER->GetCommandManager()->GetCommandList()->ResourceBarrier(1, &transition);
		}
	};
}