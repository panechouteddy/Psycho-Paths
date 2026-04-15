#pragma once

struct ID3D12DescriptorHeap;

namespace nam
{
	class RenderManager;

	struct DescriptorHeapsWrapper
	{
		UINT m_rtvDescriptorSize = 0;
		UINT m_dsvDescriptorSize = 0;
		UINT m_cbvSrvUavDescriptorSize = 0;

		ID3D12DescriptorHeap* m_rtvHeap = nullptr;
		ID3D12DescriptorHeap* m_dsvHeap = nullptr;

		ID3D12DescriptorHeap* m_globalHeap = nullptr;
	public:
		DescriptorHeapsWrapper() = default;
		~DescriptorHeapsWrapper();

		void UnInit();

		void CreateRTVandDescriptorHeaps();
		void SetDescriptorsHeaps();
		void SetDescriptorSizes();

		ID3D12DescriptorHeap* GetRTV_Heap() const;
		ID3D12DescriptorHeap* GetDSV_Heap() const;
		ID3D12DescriptorHeap* GetGlobal_Heap() const;

		D3D12_GPU_DESCRIPTOR_HANDLE GetUAVHandle(UINT offset) const;
		D3D12_GPU_DESCRIPTOR_HANDLE GetSRVHandle(UINT offset) const; 
	};
}
