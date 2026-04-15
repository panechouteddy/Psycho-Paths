#pragma once
#include "Common/d3dx12.h"
#include <ContainerSequential.h>
#include <BaseTypes.h>

#include "Constants.h"

//0 -> b0 = cbufferPerObj
//1 -> b1 = cbufferPerFrame
//2+ -> t0 = textures

namespace nam
{
	enum RootSignatureType : u8
	{
		UnorderedAccessView,
		ConstantBufferView,
		ShaderResourceView,
	};

	struct DescriptorTableDesc
	{
		D3D12_DESCRIPTOR_RANGE_TYPE m_type; // SRV, UAV, CBV
		UINT m_numDescriptors;
		UINT m_baseShaderRegister;

		DescriptorTableDesc(RootSignatureType type, UINT descriptorsAmount, UINT baseShaderRegister);
	};

	struct RootSignature
	{
		ID3D12RootSignature* mp_get = nullptr;

		~RootSignature();

		static RootSignature* Create(Vector<RootSignatureType> simpleParams, Vector<DescriptorTableDesc> descTablesParams); // USE THE ENUM RootSignatureType to choose what do you want
	};
}