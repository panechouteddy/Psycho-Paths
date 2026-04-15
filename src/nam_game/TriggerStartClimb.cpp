#include "pch.h"
#include "TriggerStartClimb.h"

void TriggerStartClimb::OnInit()
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

	//GetComponent<BoxColliderComponent>().CreateManualBox({ 0.5f, 1, 0.5f });
}
	
void TriggerStartClimb::OnStart()
{
}

void TriggerStartClimb::OnUpdate()
{
}

void TriggerStartClimb::OnCollision(u32 self, u32 other, const CollisionInfo& collisionInfo)
{

}
