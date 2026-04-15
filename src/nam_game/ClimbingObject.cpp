#include "pch.h"
#include "ClimbingObject.h"
#include "TriggerStartClimb.h"
#include "TriggerFinishClimb.h"

void ClimbingObject::OnInit()
{
	SetBehavior();
	SetTag((int)TagObject::_ClimbObject);

	m_size = { 3, 0.15f, 0.15f };

	Mesh* p_objectMesh = RENDER_ITEM_MANAGER.CreateRenderItem<Mesh>();
	p_objectMesh->BuildBox(m_size, { 1, 1, 1, 1 });
	p_objectMesh->SetTexture((int)TagTexture::_Floor);

	//To Remove
	MeshRendererComponent ObjectMeshComp;
	ObjectMeshComp.SetMeshInstance(p_objectMesh);
	AddComponent(ObjectMeshComp);

	AddBoxCollider();

	//mp_triggerStart = GetScene()->CreateGameObject<TriggerStartClimb>();
	//mp_triggerFinish = GetScene()->CreateGameObject<TriggerFinishClimb>();
	
}

void ClimbingObject::OnStart()
{
}

void ClimbingObject::OnUpdate()
{
}

void ClimbingObject::OnCollision(u32 self, u32 other, const CollisionInfo& collisionInfo)
{
}

XMFLOAT3 ClimbingObject::GetSize()
{
	return m_size;
}
