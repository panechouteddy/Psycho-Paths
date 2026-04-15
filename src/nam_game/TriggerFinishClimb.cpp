#include "pch.h"
#include "TriggerFinishClimb.h"


void TriggerFinishClimb::OnInit()
{
	SetBehavior();

	//To remove
	Mesh* p_objectMesh = RENDER_ITEM_MANAGER.CreateRenderItem<Mesh>();
	p_objectMesh->BuildBox({ 0.15f, 1, 0.25f }, { 1, 1, 1, 1 });
	p_objectMesh->SetTexture((int)TagTexture::_Floor);

	MeshRendererComponent ObjectMeshComp;
	ObjectMeshComp.SetMeshInstance(p_objectMesh);
	AddComponent(ObjectMeshComp);
	AddBoxCollider();
}

void TriggerFinishClimb::OnStart()
{
}

void TriggerFinishClimb::OnUpdate()
{
}

void TriggerFinishClimb::OnCollision(u32 self, u32 other, const CollisionInfo& collisionInfo)
{
	GameObject* gameObject = GetScene()->GetGameObject(other);
	int tagOther = gameObject->GetTag();

	if (tagOther == (int)TagObject::_Player)
	{
		SetActive(false);
	}
}
