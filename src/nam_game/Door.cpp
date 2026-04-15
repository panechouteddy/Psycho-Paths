#include "pch.h"
#include "Door.h"

#include "InteractComponent.h"

void Door::OnInit()
{
	SetBehavior();
	SetTag((int)TagObject::_WallIsolate);

	AddBoxCollider((int)TagCollision::_Decor);

	PhysicComponent pc;
	pc.m_mass = 50;
	pc.m_bounciness = 0.f;
	pc.m_isKinematic = true;
	pc.m_useGravity = false;
	AddComponent(pc);

	m_animTimer.Init(0.5f);
}

void Door::OnStart()
{
	if (m_isClosed == false)
		ToggleDoor();

	m_canOpen = m_canOpenFirst;

	if (m_dims.width == 0 && m_dims.depth == 0)
	{
		Mesh* p_mesh = GetComponent<MeshRendererComponent>().mp_mesh;
		m_dims = { p_mesh->m_extend.x * 2.f, p_mesh->m_extend.y * 2.f, p_mesh->m_extend.z * 2.f };
	}

	int pivotSens = m_rightPivot ? 1 : -1;

	m_pivotPoint = mp_transform->GetWorldPosition();

	XMFLOAT3 axis = {};
	float halfSize = -1;

	if (m_dims.width > m_dims.depth)
	{
		axis = mp_transform->GetLocalAxis(LocalAxis::_Right); // X
		halfSize = m_dims.width * 0.5f;
	}
	else
	{
		axis = mp_transform->GetLocalAxis(LocalAxis::_Forward); // Z
		halfSize = m_dims.depth * 0.5f;
	}

	m_pivotPoint.x += axis.x * pivotSens * halfSize;
	m_pivotPoint.y += axis.y * pivotSens * halfSize;
	m_pivotPoint.z += axis.z * pivotSens * halfSize;
}

void Door::OnUpdate()
{
	float dt = CHRONO.GetScaledDeltaTime();

	XMFLOAT3 pos = mp_transform->GetWorldPosition();

	if (m_isOnAnimation)
	{
		HandleAnimation();
	}
	else
	{
		m_animTimer.ResetProgress();
	}
}

void Door::OnEnd()
{
}

void Door::OnCollision(u32 self, u32 other, const CollisionInfo& collisionInfo)
{
}

void Door::OnDestroy()
{
}

void Door::SetProperties(Dimensions dimensions, bool rightPivot, bool inner, float maxAngle, float animDuration, bool canOpen)
{
	m_dims = dimensions;

	m_rightPivot = rightPivot;
	m_inner = inner;
	m_maxAngle = maxAngle;
	m_canOpen = canOpen;
	m_canOpenFirst = canOpen;

	if (animDuration > 0)
		m_animTimer.SetTargetTime(animDuration);

	AddComponent<InteractComponent>(InteractComponent());

	if (HasComponent<MeshRendererComponent>())
		return;

	Mesh* mp_mesh = RENDER_ITEM_MANAGER.CreateRenderItem<Mesh>();
	mp_mesh->BuildBox({ m_dims.width, m_dims.height, m_dims.depth }, { 1, 1, 1, 1 });
	mp_mesh->SetTexture((int)TagTexture::_Wood);

	MeshRendererComponent meshComp;
	meshComp.SetMeshInstance(mp_mesh);
	AddComponent(meshComp);

}

void Door::CanOpen(bool canOpen)
{
	m_canOpen = canOpen;
}

void Door::ToggleDoor()
{
	InteractComponent& ic = GetComponent<InteractComponent>();
	if (m_canOpen == false)
	{
		ic.m_indicator = "x";
		ic.m_indicatorColor = { 1, 0, 0, 0.75f };
		AudioManager::PlaySound("DoorLocked");
		return;
	}
	else
	{
		ic.m_indicator = 'o';
		ic.m_indicatorColor = ic.m_defaultColor;
	}

	GameObject* p_player = GetScene()->GetFirstGameObjectWithTag((int)TagObject::_Player);

	if (p_player == nullptr)
		return;

	XMFLOAT3 playerF = p_player->mp_transform->GetLocalAxis(_Forward);
	XMVECTOR playerFV = XMLoadFloat3(&playerF);

	XMFLOAT3 doorF = mp_transform->GetLocalAxis(_Forward);
	XMVECTOR doorFV = XMLoadFloat3(&doorF);

	float dot = XMVectorGetX(XMVector3Dot(playerFV, doorFV));

	if (m_canOpen == false)
		return;

	if (m_isOnAnimation)
		return;

	m_isOnAnimation = true;
	AudioManager::PlaySound("DoorOpen");

	if (m_isClosed == false)
		return;

	if (dot > 0)
		m_inner = true;
	else
		m_inner = false;
}

void Door::HandleAnimation()
{
	float dt = CHRONO.GetScaledDeltaTime();

	int sens = m_isClosed ? -1 : 1;
	int pivotSens = m_rightPivot ? -1 : 1;

	int inner = m_inner ? 1 : -1;

	float dtheta = (m_maxAngle * dt) / m_animTimer.GetTargetTime();

	XMFLOAT3 up = mp_transform->GetLocalAxis(_Up);

	if (up.x + up.y + up.z == 0.f)
		up = { 1, 0, 0 };

	mp_transform->RotateAroundWorld(m_pivotPoint, up, sens * pivotSens * inner * dtheta);
	m_animTimer.Update(CHRONO.GetScaledDeltaTime());

	if (m_animTimer.IsTargetReached())
	{
		m_isOnAnimation = false;
		m_isClosed = !m_isClosed;
		m_animTimer.ResetProgress();
		return;
	}
}
