#pragma once
#include "Common/UploadBuffer.h"
#include "DataShader.h"

namespace nam
{
	enum CBufferType
	{
		_Obj3D,
		_ViewProj,

		_Lights,
		_Material,

		_Skinning,

		_Sprite2D,
		_Color2DOrtho,

		_Particle,
		_ParticleCompute
	};

	struct ConstantBuffersDatas
	{
		UploadBuffer<CBufferObj3D>* mp_Obj3D_CB = nullptr;
		UploadBuffer<CBufferViewProj3D>* mp_viewProjCB = nullptr;

		UploadBuffer<CBufferLights>* mp_lightCB = nullptr;
		UploadBuffer<CBufferMaterial>* mp_matCB = nullptr;

		UploadBuffer<CBUfferSkinning>* mp_skinningCB = nullptr;

		UploadBuffer<CBufferSprite2D>* mp_Sprite2D_CB = nullptr;
		UploadBuffer<CBufferOrtho2D>* mp_orthoCB = nullptr;

	public:
		ConstantBuffersDatas() = default;
		~ConstantBuffersDatas();

		void UnInit();

		D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAdressOf(CBufferType type) const;

		void CreateMinimal();

		void CreateOthers();
	};
}
