#include "pch.h"
#include "Locker.h"
#include "InteractComponent.h"

void Locker::OnInit()
{
	SetBehavior();
	SetTag((int)TagObject::_Locker);

	AddBoxCollider();

	InteractComponent ic;
	ic.InitInteract([this]()
		{
			XMFLOAT3 lockerPos = mp_transform->GetWorldPosition();
			Player* player = (Player*)SCENE_MANAGER.GetScene((int)TagScene::_Main)->GetFirstGameObjectWithTag((int)TagObject::_Player);
			player->SwitchHideState();

			player->mp_transform->SetWorldPosition(lockerPos);
			Flashlight* flashlight = (Flashlight*)SCENE_MANAGER.GetScene((int)TagScene::_Main)->GetFirstGameObjectWithTag((int)TagObject::_Flashlight);
			flashlight->SetFlashLightOff();
		});

	AddComponent(ic);
}

void Locker::OnStart()
{
	
}

void Locker::OnUpdate()
{

}

void Locker::OnCollision(u32 self, u32 other, const CollisionInfo& collisionInfo)
{

}
