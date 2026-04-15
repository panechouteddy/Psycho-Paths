#include "pch.h"
#include "NoiseSystem.h"

#include <App.h>

#include "NoiseComponent.h"

#include <DirectXMath.h>
#include <DirectXCollision.h>

#include "Monster.h"

void NoiseSystem::SetMonsterTag(int tag)
{
	m_monsterTag = tag;
}

void NoiseSystem::Update(Ecs& ecs)
{
	Scene* p_scene = SCENE_MANAGER.GetScene((int)TagScene::_Main);

	if (p_scene->IsActive() == false)
		return;

	GameObject* p_monster = p_scene->GetFirstGameObjectWithTag(m_monsterTag);
	XMFLOAT3 monsterPos = { 0, 0, 0 };

	if(p_monster)
		monsterPos = p_monster->mp_transform->GetWorldPosition();

	ecs.ForEach<NoiseComponent, TransformComponent>([&](u32& entity, NoiseComponent& nc, TransformComponent& transform)
		{
			transform.UpdateWorldData();

			if (nc.m_activated == false)
				return;

			nc.m_activated = false;

			XMFLOAT3 soundPos = transform.GetWorldPosition();
			AudioManager::PlaySound(nc.m_soundName, nc.m_soundScale, soundPos, true);

			p_scene->GetGameObject(entity)->SetActive(false);

			if (p_monster == nullptr)
				return;

			if (IsMonsterHearingSound(p_scene, monsterPos, soundPos, nc.m_soundRange))
			{
				Monster* p_monsterCast = (Monster*)p_monster;
				p_monsterCast->TriggerFollow();
			}
		});
}

bool NoiseSystem::IsMonsterHearingSound(Scene* p_scene, XMFLOAT3 monsterPos, XMFLOAT3 soundPos, float soundRange)
{
	float sqrSoundDist = Maths::GetSquareDistance(monsterPos, soundPos);

	Vector<GameObject*> objs = p_scene->GetAllGameObjectsWithTag({ (int)TagObject::_WallIsolate }, true);

	bool blocked = false;
	for (GameObject* obj : objs)
	{
		if (obj->HasComponent<BoxColliderComponent>() == false)
			continue;

		BoxColliderComponent& bc = obj->GetComponent<BoxColliderComponent>();
		DirectX::BoundingOrientedBox box = bc.m_box;

		float hitDist = 0;

		XMFLOAT3 dir = Maths::GetDirection(soundPos, monsterPos);

		if (box.Intersects(XMLoadFloat3(&soundPos), XMLoadFloat3(&dir), hitDist) && hitDist * hitDist < sqrSoundDist)
		{
			blocked = true;
			break;
		}
	}

	if (blocked)
		return false;

	return sqrSoundDist <= soundRange * soundRange;
}
