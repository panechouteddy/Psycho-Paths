#include "pch.h"
#include "InteractorSystem.h"
#include "InteractComponent.h"

#include "GameKeybinds.h"
#include "UIObject.h"

namespace nam
{
	void InteractorSystem::InitTextInteract()
	{
		mp_indicator = SCENE_MANAGER.GetScene((int)TagScene::_Main)->CreateGameObject<UIObject>();
		mp_indicator->CreateText({ 0, 0 }, "o", 2, { 1, 1, 1, 1 });
		mp_indicator->mp_text->SetDrawLayer(-1);
	}

	void InteractorSystem::SetPositiontoInteract(TransformComponent& tc)
	{
		XMFLOAT3 transformPos = tc.GetWorldPosition();
		XMVECTOR worldPos = XMLoadFloat3(&transformPos);

		XMMATRIX world = XMLoadFloat4x4(&tc.GetWorldMatrix());
		XMMATRIX view = XMLoadFloat4x4(&APP->GetCamera()->GetComponent<CameraComponent>().m_view);
		XMMATRIX proj = XMLoadFloat4x4(&APP->GetCamera()->GetComponent<CameraComponent>().m_proj);

		XMVECTOR projected = XMVector3Project(worldPos, 0.0f, 0.0f,
			(float)WINDOW->m_startWidth, (float)WINDOW->m_startHeight,
			0.0f, 1.0f,
			proj,
			view,
			XMMatrixIdentity()
		);

		XMFLOAT3 screenPos = {};
		XMStoreFloat3(&screenPos, projected);

		XMFLOAT2 worldToScreen = { screenPos.x, screenPos.y };

		mp_indicator->SetUiPosition({ worldToScreen.x , worldToScreen.y });
		mp_indicator->SetActive(true);
	}

	void InteractorSystem::Update(Ecs& ecs)
	{
		m_objectDetected = false;
		mp_indicator->SetActive(false);

		if (CHRONO.IsFrozen())
			return;

		ecs.ForEach<InteractComponent, TransformComponent>(
			[&](u32 e, InteractComponent& c, TransformComponent& t)
			{
				if (m_objectDetected)
					return;

				m_objectDetected = IsInTheDetectionCone(e, t);
				if (m_objectDetected)
				{
					Text* p_text = mp_indicator->GetComponent<TextRendererComponent>().mp_text;
					p_text->SetText(c.m_indicator);
					p_text->SetColor(c.m_indicatorColor);

					SetPositiontoInteract(t);
					if (Input::IsKeyDown(GameKeybinds::GetKey(KeyAction::_Interact)) || Input::GetGamePad(0)->IsButtonDown(GameKeybinds::GetButton(KeyAction::_Interact)))
					{
						auto a = SCENE_MANAGER.GetScene((int)TagScene::_Main)->GetGameObject(e);

						if (c.OnTrigger != nullptr)
						{
							c.OnTrigger();
						}
					}
				}
			}
		);
	}
	bool InteractorSystem::IsInTheDetectionCone(u32 e, TransformComponent& tc)
	{
		GameObject* object = SCENE_MANAGER.GetScene((int)TagScene::_Main)->GetGameObject(e);

		BoxColliderComponent* boxCollider = &object->GetComponent<BoxColliderComponent>();
		TransformComponent& camF = *APP->GetCamera()->mp_transform;

		if (boxCollider == nullptr)
			return false;

		DirectX::BoundingOrientedBox box = boxCollider->m_box;
		XMFLOAT3 oPos = tc.GetWorldPosition();
		XMFLOAT3 camPos = camF.GetWorldPosition();
		XMVECTOR camPosV = XMLoadFloat3(&camPos);
		XMVECTOR objPosV = XMLoadFloat3(&oPos);

		XMVECTOR dirV = XMVector3Normalize(objPosV - camPosV);

		XMFLOAT3 forward = camF.GetLocalAxis(_Forward);
		XMVECTOR forwardV = XMLoadFloat3(&forward);

		//Notes :
		//dot = 1 -> cos(0) = colinear
		//dot = 0 -> cos(90) = orthogonal
		//dot = -1 -> cos(180) = oppose

		float dot = XMVectorGetX(XMVector3Dot(forwardV, dirV));
		float fov = XM_PIDIV4 * 0.5f;
		float minDot = cosf(fov * 0.5f);

		bool result = (dot >= minDot);

		if (result == false)
			return false;

		XMFLOAT3 dir = Maths::GetDirection(camF.GetWorldPosition(), tc.GetWorldPosition());
		XMVECTOR VectorDir = XMLoadFloat3(&dir);

		float objPlayerDist = sqrtf(Maths::GetSquareDistance(camF.GetWorldPosition(), tc.GetWorldPosition()));

		float hitDist = 0;
		if (!box.Intersects(XMLoadFloat3(&camPos), dirV, hitDist) && hitDist < objPlayerDist)
			return false;

		if (hitDist > 1.5f)
			return false;

		return true;

	}
}
