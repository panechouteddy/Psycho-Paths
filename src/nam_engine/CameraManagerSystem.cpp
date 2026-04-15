#include "pch.h"
#include "CameraManagerSystem.h"

#include "RenderManager.h"

#include "TransformComponent.h"
#include "CameraComponent.h"

#include "DataShader.h"
#include "Entity.h"
#include "App.h"

namespace nam
{
	void CameraManagerSystem::Update(Ecs& ecs)
	{
		// 3D
		Handle3D(ecs);

		// 2D
		Handle2D(ecs);
	}

	void CameraManagerSystem::Handle3D(Ecs& ecs)
	{
		TransformComponent* tc = nullptr;
		CameraComponent* cc = nullptr;

		ecs.ForEach<CameraComponent, TransformComponent>(
			[&](u32 e, CameraComponent& c, TransformComponent& t) {
				if (tc != nullptr || cc != nullptr)
					return;

				tc = &t;
				cc = &c;
			}
		);

		if (tc == nullptr || cc == nullptr)
			return;

		tc->UpdateWorldData();

		if (cc == nullptr)
			return;

		tc->UpdateWorldData();

		CBufferViewProj3D viewProj;
		viewProj.m_cameraPos = tc->GetWorldPosition();

		DirectX::XMMATRIX camWorld = XMLoadFloat4x4(&tc->GetWorldMatrix());
		DirectX::XMMATRIX viewMat = XMMatrixInverse(nullptr, camWorld); // view = inverse of world

		XMStoreFloat4x4(&cc->m_view, viewMat);
		cc->UpdateFrustum();

		XMStoreFloat4x4(&viewProj.m_view, XMMatrixTranspose(viewMat));
	
		DirectX::XMStoreFloat4x4(&viewProj.m_proj, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&cc->m_proj)));
		viewProj.time = static_cast<float>(App::Get()->GetChrono().GetScaledTotalTime());
		viewProj.deltaTime = static_cast<float>(App::Get()->GetChrono().GetScaledDeltaTime());

		RENDER_MANAGER->GetConstantBuffersDatas()->mp_viewProjCB->CopyData(0, viewProj);
	}

	void CameraManagerSystem::Handle2D(Ecs& ecs)
	{
		Window* p_window = RENDER_MANAGER->GetWindow();

		DirectX::XMFLOAT2 windowSize = { (float)p_window->m_clientWidth, (float)p_window->m_clientHeight };

		if (windowSize.x <= 0 || windowSize.y <= 0)
			return;

		CBufferOrtho2D proj;
		DirectX::XMMATRIX ortho = DirectX::XMMatrixOrthographicOffCenterLH(0.0f, windowSize.x, windowSize.y, 0.0f, 0.0f, 1.0f);

		DirectX::XMStoreFloat4x4(&proj.m_ortho, DirectX::XMMatrixTranspose(ortho));
		proj.time = static_cast<float>(App::Get()->GetChrono().GetScaledTotalTime());

		RENDER_MANAGER->GetConstantBuffersDatas()->mp_orthoCB->CopyData(0, proj);
	}
}