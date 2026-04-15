#include "pch.h"
#include "ConstantBuffersDatas.h"
#include "RenderManager.h"

namespace nam
{
	ConstantBuffersDatas::~ConstantBuffersDatas()
	{
		UnInit();
	}

	void ConstantBuffersDatas::UnInit()
	{
		if (mp_Obj3D_CB)
		{
			delete mp_Obj3D_CB;
			mp_Obj3D_CB = nullptr;
		}

		if (mp_Sprite2D_CB)
		{
			delete mp_Sprite2D_CB;
			mp_Sprite2D_CB = nullptr;
		}

		if (mp_viewProjCB)
		{
			delete mp_viewProjCB;
			mp_viewProjCB = nullptr;
		}

		if (mp_orthoCB)
		{
			delete mp_orthoCB;
			mp_orthoCB = nullptr;
		}

		if (mp_lightCB)
		{
			delete mp_lightCB;
			mp_lightCB = nullptr;
		}

		if (mp_matCB)
		{
			delete mp_matCB;
			mp_matCB = nullptr;
		}
	}

	D3D12_GPU_VIRTUAL_ADDRESS ConstantBuffersDatas::GetGPUVirtualAdressOf(CBufferType type) const
	{
		D3D12_GPU_VIRTUAL_ADDRESS adress = {};

		switch (type)
		{
		case _Obj3D:
			adress = mp_Obj3D_CB->Resource()->GetGPUVirtualAddress();
			break;

		case _ViewProj:
			adress = mp_viewProjCB->Resource()->GetGPUVirtualAddress();
			break;

		case _Lights:
			adress = mp_lightCB->Resource()->GetGPUVirtualAddress();
			break;

		case _Material:
			adress = mp_matCB->Resource()->GetGPUVirtualAddress();
			break;

		case _Skinning:
			adress = mp_skinningCB->Resource()->GetGPUVirtualAddress();

		case _Sprite2D:
			adress = mp_Sprite2D_CB->Resource()->GetGPUVirtualAddress();
			break;

		case _Color2DOrtho:
			adress = mp_orthoCB->Resource()->GetGPUVirtualAddress();
			break;

		default:
			break;
		}

		return adress;
	}

	void ConstantBuffersDatas::CreateMinimal()
	{
		ID3D12Device* p_device = RENDER_MANAGER->GetDevice();

		mp_viewProjCB = new UploadBuffer<CBufferViewProj3D>(p_device, 1, true);

		mp_Sprite2D_CB = new UploadBuffer<CBufferSprite2D>(p_device, 1024, true);
		mp_orthoCB = new UploadBuffer<CBufferOrtho2D>(p_device, 1, true);

		mp_matCB = new UploadBuffer<CBufferMaterial>(p_device, 128, true);

		CBufferMaterial test;
		test.m_albedo = { 1, 1, 1 };
		test.m_metalness = 0.f;
		test.m_roughness = 1.f;

		mp_matCB->CopyData(MAX_MATERIALS - 1, test);
	}

	void ConstantBuffersDatas::CreateOthers()
	{
		ID3D12Device* p_device = RENDER_MANAGER->GetDevice();

		mp_Obj3D_CB = new UploadBuffer<CBufferObj3D>(p_device, 1024, true);
		mp_lightCB = new UploadBuffer<CBufferLights>(p_device, 1, true);

		mp_skinningCB = new UploadBuffer<CBUfferSkinning>(p_device, 1024, true);
	}
}
