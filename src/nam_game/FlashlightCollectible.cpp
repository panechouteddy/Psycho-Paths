#include "pch.h"
#include "FlashlightCollectible.h"

#include "InteractComponent.h"
#include "ItemsList.h"

void FlashlightCollectible::OnInit()
{
	SetProperties(ItemListName::Flashlight, ItemListDesc::Flashlight, true);
}

void FlashlightCollectible::OnStart()
{
}

void FlashlightCollectible::OnUpdate()
{
}

void FlashlightCollectible::OnCollision(u32 self, u32 other, const CollisionInfo& collisionInfo)
{
}

void FlashlightCollectible::OnUse()
{
	XMFLOAT3 lockerPos = mp_transform->GetWorldPosition();
	Player* player = (Player*)SCENE_MANAGER.GetScene((int)TagScene::_Main)->GetFirstGameObjectWithTag((int)TagObject::_Player);
	player->SetHasFlashlight(true);
}
