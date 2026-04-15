#include "pch.h"
#include "InputLayout.h"

namespace nam
{
	void InputLayout::Add(const std::string& semantic, UINT semanticIndex, DXGI_FORMAT format, UINT inputSlot, UINT alignedByteOffset)
	{
		InputLayoutElementDesc desc;
		desc.semanticName = semantic;
		desc.format = format;
		desc.inputSlot = inputSlot;
		desc.semanticIndex = semanticIndex;
		desc.alignedByteOffset = alignedByteOffset;

		m_elements.push_back(desc);

		m_dirty = true;
	}

	void InputLayout::Build()
	{
		m_rawElements.clear();

		for (auto& e : m_elements)
		{
			D3D12_INPUT_ELEMENT_DESC desc = {};
			desc.SemanticName = e.semanticName.c_str();
			desc.SemanticIndex = e.semanticIndex;
			desc.Format = e.format;
			desc.InputSlot = e.inputSlot;
			desc.AlignedByteOffset = e.alignedByteOffset;
			desc.InputSlotClass = e.inputSlotClass;
			desc.InstanceDataStepRate = e.instanceDataStepRate;

			m_rawElements.push_back(desc);
		}

		m_dirty = false;
	}

	D3D12_INPUT_LAYOUT_DESC InputLayout::GetDesc() const
	{
		if (m_dirty)
		{
			FatalError("InputLayout not clean ! Try to call Build\n");
		}

		m_d3d12Desc.NumElements = static_cast<UINT>(m_elements.size());
		m_d3d12Desc.pInputElementDescs = m_rawElements.data();

		return m_d3d12Desc;
	}
}