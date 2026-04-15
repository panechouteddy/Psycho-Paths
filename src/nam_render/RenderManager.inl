#include "Common/UploadBuffer.h"

namespace nam
{
	template<typename T>
	inline void RenderManager::SetConstantBuffer(size_t index, UploadBuffer<T>* buffer)
	{
		mp_commandManager.GetCommandList()->SetGraphicsRootConstantBufferView(index, buffer->Resource()->GetGPUVirtualAddress());
	}
}
