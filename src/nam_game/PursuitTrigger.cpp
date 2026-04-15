#include "pch.h"
#include "PursuitTrigger.h"

#include "Monster.h"
#include "Player.h"

void PursuitTrigger::OnInit()
{
	AddBoxCollider((int)TagCollision::_Decor);
}

void PursuitTrigger::OnStart()
{
	SetActive(true);
}

void PursuitTrigger::OnCollision(u32 self, u32 other, const CollisionInfo& collisionInfo)
{
	GameObject* gameObject = GetScene()->GetGameObject(other);
	int tagOther = gameObject->GetTag();

	if (tagOther == (int)TagObject::_Player)
	{
		SetActive(false);
		Player* p_player = (Player*)gameObject;
		p_player->SetPursuitMode(true);

		Monster* m = (Monster*)GetScene()->GetFirstGameObjectWithTag((int)TagObject::_Monster);
		m->ActivatePursuit();
		m->mp_transform->SetWorldPosition({ 68.f, 0.3f, 0.5f });
	}
}