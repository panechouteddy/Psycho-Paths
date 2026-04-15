#include "pch.h"
#include "EndscreenTrigger.h"

void EndscreenTrigger::OnInit()
{
	AddBoxCollider((int)TagCollision::_Decor);
}

void EndscreenTrigger::OnStart()
{
	SetActive(true);
}

void EndscreenTrigger::OnCollision(u32 self, u32 other, const CollisionInfo& collisionInfo)
{
	GameObject* gameObject = GetScene()->GetGameObject(other);
	int tagOther = gameObject->GetTag();

	if (tagOther == (int)TagObject::_Player)
	{
		SetActive(false);
		SCENE_MANAGER.SetExclusiveActiveScene((int)TagScene::_EndCreditsScene);
	}
}
