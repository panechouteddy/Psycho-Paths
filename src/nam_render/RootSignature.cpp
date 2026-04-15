#include "pch.h"
#include "RootSignature.h"
#include "RenderManager.h"

namespace nam
{
	DescriptorTableDesc::DescriptorTableDesc(RootSignatureType type, UINT descriptorsAmount, UINT baseShaderRegister)
	{
		switch (type)
		{
		case UnorderedAccessView:
			m_type = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
			break;

		case ConstantBufferView:
			m_type = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
			break;

		case ShaderResourceView:
			m_type = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			break;

		default:
			m_type = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
			break;
		}

		m_numDescriptors = descriptorsAmount;
		m_baseShaderRegister = baseShaderRegister;
	}

	RootSignature::~RootSignature()
	{
		if (mp_get)
		{
			mp_get->Release();
			mp_get = nullptr;
		}
	}

	RootSignature* RootSignature::Create(Vector<RootSignatureType> simpleParams, Vector<DescriptorTableDesc> descTablesParams)
	{
		RootSignature* p_rootSignature = new RootSignature();

		UINT amount = static_cast<UINT>(simpleParams.size() + descTablesParams.size());
		Vector<CD3DX12_ROOT_PARAMETER> slotRootParameter(amount);

		UINT index = 0;

		UINT indexUAV = 0;
		UINT indexCBV = 0;
		UINT indexSRV = 0;

		for (RootSignatureType& type : simpleParams)
		{
			switch (type)
			{
			case UnorderedAccessView:
				slotRootParameter[index].InitAsUnorderedAccessView(indexUAV);
				indexUAV++;
				break;

			case ConstantBufferView:
				slotRootParameter[index].InitAsConstantBufferView(indexCBV);
				indexCBV++;
				break;

			case ShaderResourceView:
				slotRootParameter[index].InitAsShaderResourceView(indexSRV);
				indexSRV++;
				break;
			}

			index++;
		}

		Vector<D3D12_DESCRIPTOR_RANGE> ranges(descTablesParams.size());

		for (size_t i = 0; i < descTablesParams.size(); i++)
		{
			ranges[i].RangeType = descTablesParams[i].m_type;
			ranges[i].NumDescriptors = descTablesParams[i].m_numDescriptors;
			ranges[i].BaseShaderRegister = descTablesParams[i].m_baseShaderRegister;
			ranges[i].RegisterSpace = 0;
			ranges[i].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			slotRootParameter[index + i].InitAsDescriptorTable(1, &ranges[i]);
		}

		// Static sampler
		CD3DX12_STATIC_SAMPLER_DESC sampler(
			0, // s0
			D3D12_FILTER_MIN_MAG_MIP_LINEAR,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

		// A root signature is an array of root parameters.
		CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(amount, slotRootParameter.data(), 1, &sampler,
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
		ComPtr<ID3DBlob> serializedRootSig = nullptr;
		ComPtr<ID3DBlob> errorBlob = nullptr;
		HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
			serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

		if (errorBlob != nullptr)
		{
			::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
		}
		ThrowIfFailed(hr);

		ThrowIfFailed(RENDER_MANAGER->GetDevice()->CreateRootSignature(
			0,
			serializedRootSig->GetBufferPointer(),
			serializedRootSig->GetBufferSize(),
			IID_PPV_ARGS(&p_rootSignature->mp_get)));

		return p_rootSignature;
	}
}
