#include "pch.h"
#include "Monster.h"

void Monster::OnInit()
{
	m_name = "Monster";
	SetBehavior();
	SetTag((int)TagObject::_Monster);

	m_animTimer.Init(1.f);
	mp_mesh = RENDER_ITEM_MANAGER.CreateRenderItem<Mesh>();

	mp_mesh->LoadObj(L"../../res/Assets/Stalker/Stalker.obj");
	mp_mesh->SetTexture((int)TagTexture::_Stalker);

	MeshRendererComponent meshComp;
	meshComp.SetMeshInstance(mp_mesh);
	AddComponent(meshComp);
	AddBoxCollider();
		
	m_stateManager.Init({ States::_IDLE, States::_PATROL, States::_FOLLOW,States::_PURSUIT });

	m_dontSeePlayerTimer.Init(5.f);
	m_afkTimer.Init(2.f);
}

void Monster::OnStart()
{
	if (HasComponent<PathFindingComponent>())
		mp_pathComp = &GetComponent<PathFindingComponent>();
	else
		mp_pathComp = nullptr;

	m_stateManager.SetExclusiveActiveState(States::_PATROL);

	m_afkTimer.ResetProgress();
	m_dontSeePlayerTimer.ResetProgress();

	m_isPlayerCaught = false;
	m_animTimer.ResetProgress();

	mp_transform->SetWorldYPR({ 0, 0, 0 });

	SetSeek(false);
}

void Monster::OnUpdate()
{
	CheckStates();

	if (m_isPlayerCaught)
	{
		if (m_animTimer.IsTargetReached())
		{
			SCENE_MANAGER.SetExclusiveActiveScene((int)TagScene::_GameOver);
			return;
		}

		HandleJumpscareAnimation();

		m_animTimer.Update(CHRONO.GetScaledDeltaTime());
		return;
	}

	GameObject* p_player = GetScene()->GetFirstGameObjectWithTag((int)TagObject::_Player);

	if (p_player == nullptr)
		return;

	XMFLOAT3 m = mp_transform->GetWorldPosition();
	XMFLOAT3 p = p_player->mp_transform->GetWorldPosition();

	XMFLOAT3 lookDir = Maths::GetDirection(m, p);
	lookDir.y = 0.f;

	XMStoreFloat3(&lookDir, XMVector3Normalize(XMLoadFloat3(&lookDir)));

	float dt = CHRONO.GetScaledDeltaTime();

	if (m_seek)
	{
		if (m_dontSeePlayerTimer.IsTargetReached())
		{
			ToPatrol();
			return;
		}

		if (IsSeeingPlayer(m, p, p_player, m_visionRange))
			m_dontSeePlayerTimer.ResetProgress();
		else
			m_dontSeePlayerTimer.Update(dt);

		return;
	}

	if (m_afkTimer.IsTargetReached())
	{
		if (IsSeeingPlayer(m, p, p_player, m_visionRange))
		{
			TriggerFollow();
		}
	}
	else
		m_afkTimer.Update(dt);

}

void Monster::OnEnd()
{
}

void Monster::CheckStates()
{
	if (m_stateManager.IsActiveState(States::_IDLE) == true)
		MonsterIdle();

	if (m_stateManager.IsActiveState(States::_PATROL) == true)
		MonsterPatrol();

	if (m_stateManager.IsActiveState(States::_FOLLOW) == true)
		MonsterFollow();

	if (m_stateManager.IsActiveState(States::_PURSUIT) == true)
		MonsterPursuit();
}

void Monster::OnCollision(u32 self, u32 other, const CollisionInfo& collisionInfo)
{
	GameObject* gameObject = GetScene()->GetGameObject(other);
	int tagOther = gameObject->GetTag();
 
	if (m_isPlayerCaught == false)
	{
		if (tagOther == (int)TagObject::_Player)
		{
			APP->SetAmbientLightColor({ 0.1f, 0, 0 });

			AudioManager::StopCurrentMusic();
			AudioManager::PlaySound("Jumpscare2", 1.f);

			Player* p_player = (Player*)gameObject;
			p_player->SetActiveUI(false);

			GameObject* p_cam = APP->GetCamera();
			XMVECTOR toMonsterV = XMLoadFloat3(&mp_transform->GetLocalAxis(_Forward));
			toMonsterV *= -1;

			XMFLOAT3 toMonster = {};
			XMStoreFloat3(&toMonster, toMonsterV);
			p_cam->mp_transform->LookToWorld(toMonster);

			m_stateManager.SetExclusiveActiveState(States::_IDLE);

			//MATERIAL_MANAGER.ChangeMaterial((int)TagMaterial::_MONSTER, -1, -1, { 1, 0.25f, 0.25f });
			//GetComponent<PhysicComponent>().m_isKinematic = true;
			gameObject->SetActive(false);
			m_isPlayerCaught = true;

			return;
		}
	}
}

void Monster::OnDestroy()
{
}

void Monster::ActivatePursuit()
{
	m_stateManager.SetExclusiveActiveState(Monster::States::_PURSUIT);
	AudioManager::PlayMusic("Intense", 1.f, true, false);
}

void Monster::SetSeek(bool state)
{
	m_seek = state;

	if (m_seek)
	{
	}
	else
	{
		m_afkTimer.ResetProgress();
	}

	m_dontSeePlayerTimer.ResetProgress();
}

void Monster::MonsterIdle()
{
	if(mp_pathComp)
		mp_pathComp->m_isInPatrol = false;
}

void Monster::MonsterPatrol()
{
	if (mp_pathComp == nullptr)
		return;

	mp_pathComp->m_isInPatrol = true;
	mp_pathComp->m_isFollowingPlayer = false;
}

void Monster::MonsterFollow()
{
	GameObject* p_player = GetScene()->GetFirstGameObjectWithTag((int)TagObject::_Player);

	if (p_player == nullptr)
		return;

	if (mp_pathComp)
	{
		mp_pathComp->m_isInPatrol = true;
		mp_pathComp->m_isFollowingPlayer = true;

		XMFLOAT3  playerPos = p_player->mp_transform->GetWorldPosition();

		Node* nodeToGo = mp_pathComp->mp_grid->GetNearestNode(playerPos);

		if (nodeToGo == nullptr)
			m_stateManager.SetExclusiveActiveState(States::_PATROL);
		else if (nodeToGo == mp_pathComp->m_pathfinder.m_End)
			return;
		else
		{
			mp_pathComp->SetPosToGo(nodeToGo);
		}
	}
	else
	{
		XMFLOAT3 lookDir = Maths::GetDirection(mp_transform->GetWorldPosition(), p_player->mp_transform->GetWorldPosition());
		lookDir.y = 0.f;

		mp_transform->LookToWorld(lookDir);
		mp_transform->MoveWorld(m_speed * CHRONO.GetScaledDeltaTime(), _Forward, { 1, 0, 1 });
	}
}

void Monster::TriggerFollow()
{
	if (m_stateManager.IsActiveState(States::_PATROL))
	{
		m_stateManager.SetExclusiveActiveState(States::_FOLLOW);
		AudioManager::PlayMusic("Seeking", 1.f, true, false);
		AudioManager::PlaySound("Laughs", 1.f, mp_transform->GetWorldPosition(), true);

		if (mp_pathComp)
			mp_pathComp->m_path.clear();

		SetSeek(true);
	}
}

void Monster::MonsterPursuit()
{
	if(mp_pathComp)
		mp_pathComp->m_isInPatrol = false;

	if (PursitIsArrived())
		m_pursuitPoints.pop();

	if (m_pursuitPoints.empty())
	{
		m_stateManager.SetExclusiveActiveState(States::_IDLE);
		return;
	}

	XMFLOAT3 dir = Maths::GetDirection(mp_transform->GetWorldPosition(), m_pursuitPoints.top().position);

	XMStoreFloat3(&dir, XMVector3Normalize(XMLoadFloat3(&dir)));
	mp_transform->LookToWorld(dir);
	mp_transform->MoveWorld(m_speed * CHRONO.GetScaledDeltaTime(), LocalAxis::_Forward);
}

void Monster::ToPatrol()
{
	if (m_stateManager.IsActiveState(States::_FOLLOW))
	{
		AudioManager::PlayMusic("Ambient2", 1.f, true, false);
		m_stateManager.SetExclusiveActiveState(States::_PATROL);
	}

	SetSeek(false);
}

bool Monster::PursitIsArrived()
{
	if (m_pursuitPoints.empty())
		return false;

	XMFLOAT3 pos = mp_transform->GetWorldPosition();
	pos.y = m_pursuitPoints.top().position.y;
	return Maths::XMFLOAT3IsEqual(pos, m_pursuitPoints.top().position, 0.50f);
}

bool Monster::IsSeeingPlayer(XMFLOAT3 m, XMFLOAT3 p, GameObject* p_player, float visionRange)
{
	if (IsPlayerInCone(m, p, p_player) == false)
		return false;

	float monsterPlayerDist = sqrtf(Maths::GetSquareDistance(m, p));

	bool blocked = false;
	Vector<GameObject*> objs = GetScene()->GetAllGameObjectsWithTag({ (int)TagObject::_Floor, (int)TagObject::_WallIsolate, (int)TagObject::_WallPaper }, true);

	for (GameObject* obj : objs)
	{
		if (obj->HasComponent<BoxColliderComponent>() == false)
			continue;

		BoxColliderComponent& bc = obj->GetComponent<BoxColliderComponent>();
		DirectX::BoundingOrientedBox box = bc.m_box;

		XMFLOAT3 dir = Maths::GetDirection(m, p);
		XMVECTOR dirV = XMLoadFloat3(&dir);

		float hitDist = 0;
		if (box.Intersects(XMLoadFloat3(&m), dirV, hitDist) && hitDist < monsterPlayerDist)
		{
			blocked = true;
			break;
		}
	}

	if (blocked)
		return false;

	return monsterPlayerDist <= visionRange;
}

bool Monster::IsPlayerInCone(XMFLOAT3 m, XMFLOAT3 p, GameObject* p_player)
{
	if (p_player->HasComponent<BoxColliderComponent>() == false)
		return false;

	XMVECTOR monsterPos = XMLoadFloat3(&m);
	XMVECTOR playerPos = XMLoadFloat3(&p);

	XMVECTOR dirV = XMVector3Normalize(playerPos - monsterPos);

	XMFLOAT3 forward = mp_transform->GetLocalAxis(_Forward);
	XMVECTOR forwardV = XMLoadFloat3(&forward);

	//Notes :
	//dot = 1 -> cos(0) = colinear
	//dot = 0 -> cos(90) = orthogonal
	//dot = -1 -> cos(180) = oppose

	float dot = XMVectorGetX(XMVector3Dot(forwardV, dirV));
	float minDot = cosf(m_fov * 0.5f);

	bool result = (dot >= minDot);

	return result;
}

void Monster::HandleJumpscareAnimation()
{
	GameObject* p_cam = APP->GetCamera();
	float ratio = m_animTimer.GetProgress() / m_animTimer.GetTargetTime();
	float distanceFromPlayer = 0.4f + powf((1 - ratio), 4) * 0.5f;

	float offsetY = -GetComponent<MeshRendererComponent>().mp_mesh->m_extend.y * 1.0f;

	XMVECTOR toCamV = XMLoadFloat3(&p_cam->mp_transform->GetLocalAxis(_Forward));
	XMVECTOR camUpV = XMLoadFloat3(&p_cam->mp_transform->GetLocalAxis(_Up));

	XMFLOAT3 camPos = p_cam->mp_transform->GetWorldPosition();

	float shakeIntensity = 0.05f;
	XMFLOAT3 randomShake = { Rng::Float(-1.f, 1.f) * shakeIntensity +0.1f, Rng::Float(-1.f, 1.f) * shakeIntensity, Rng::Float(-1.f, 1.f) * shakeIntensity };
	XMVECTOR randomShakeV = XMLoadFloat3(&randomShake);

	XMVECTOR monsterPosV = XMLoadFloat3(&camPos) + XMVectorScale(toCamV, distanceFromPlayer) + camUpV * offsetY + randomShakeV;
	XMFLOAT3 monsterPos = {};
	XMStoreFloat3(&monsterPos, monsterPosV);

	mp_transform->SetWorldPosition(monsterPos);
}

void Monster::AddPursuitPoints(Vector<PursuitPoint>& pursuitpoints)
{
	m_pursuitPoints = {};

	for (PursuitPoint& pp : pursuitpoints)
	{
		m_pursuitPoints.push(pp);
	}
}
