#include "pch.h"
#include "Player.h"
#include "constants.h"
#include "UISprintBar.h"
#include "MeshRendererComponent.h"
#include "ClimbingObject.h"
#include "FootstepNoise.h"
#include "SaveManager.h"

#include "GameKeybinds.h"
#include "GeneralConfigs.h"

using namespace DirectX;

void Player::OnInit()
{
	m_name = "Player";
	SetTag((int)TagObject::_Player);
	SetBehavior();

	MeshRendererComponent meshComp;
	Mesh* p_mesh = meshComp.CreateMeshInstance();
	m_size = { 0.5f, 1.5f, 0.5f };
	p_mesh->BuildBox(m_size, { 0, 0, 1, 1 });
	p_mesh->SetVisibilty(false);

	AddComponent(meshComp);

	AddBoxCollider();

	PhysicComponent pc;
	pc.m_bounciness = 0.f;
	pc.m_isKinematic = false;
	AddComponent(pc);

	m_stateManager.Init({ States::_IDLE, States::_WALK, States::_RUN, States::_JUMP, States::_CROUCH, States::_SLIDE, States::_CLIMB, States::_HIDE,States::_DEATH });
	m_stateManager.SetExclusiveActiveState(States::_IDLE);

	mp_flashlight = GetScene()->CreateGameObject<Flashlight>();

	mp_sprintBar = GetScene()->CreateGameObject<UISprintBar>();
	mp_transform->AddChild(mp_flashlight->mp_transform);

	mp_ambientLight = GetScene()->CreateGameObject<GameObject>();
	{
		LightComponent lc;
		Light* pl = lc.CreateLightInstance();

		pl->SetToPointLight(0.25f, { 0, 0, 0 }, 9.f, { 0.9f, 0.9f, 1.f });
		mp_ambientLight->AddComponent(lc);
	}

	mp_footstepNoise = GetScene()->CreateGameObject<FootstepNoise>();

	m_isGrounded = false;
	m_doSlide = false;
	
	m_speed = PLAYER_SPEED;
	m_sprintLimit = PLAYER_RUN_LIMIT;

	m_slideTimer.Init(PLAYER_SLIDE_DURATION, false, true);
	m_timeBeforeSlide.Init(PLAYER_PAUSE_BETWEEN_SLIDE, false, true);
	m_timerBeforeClimb.Init(3.f);
}

void Player::OnStart()
{
	SaveDatas& dat = SaveManager::GetDatas();
	SetPursuitMode(false);
	SetHasFlashlight(dat.hasFlashlight);
	m_batteryCount = 0;
	GetComponent<PhysicComponent>().ResetVelocity(true, true, true);
	m_isPaused = false;
	CHRONO.SetFreezeState(m_isPaused);
	SetActiveUI(!m_isPaused);

	m_inventory.FillInventoryFromFile();
}

void Player::OnUpdate()
{
	HandlePausing();
	HandleInventory();

	if (CHRONO.IsFrozen())
		return;

	SetHasFlashlight(m_hasFlashlight);

	AudioManager::SetListenerData(mp_transform->GetWorldPosition(), mp_transform->GetLocalAxis(LocalAxis::_Forward), mp_transform->GetLocalAxis(LocalAxis::_Up));
	AppChrono& chrono = App::Get()->GetChrono();
	float dt = chrono.GetScaledDeltaTime();

	HandleInputs();
	CheckStates(dt);
	m_isGrounded = false;
	m_isClimbing = false;

	m_timerBeforeClimb.Update(dt);
}

void Player::OnCollision(u32 self, u32 other, const CollisionInfo& collisionInfo)
{
	GameObject* gameObject = GetScene()->GetGameObject(other);
	int tagOther = gameObject->GetTag();

	if (tagOther == (int)TagObject::_Floor)
	{
		mp_footstepNoise->HandleNoise(this);
		m_isGrounded = true;
	}
	else if(tagOther == (int)TagObject::_ClimbObject)
	{
		m_isClimbing = true;
		if(m_enterCollideClimb == false && m_timerBeforeClimb.IsTargetReached() == true)
		{
			ClimbingObject* co = (ClimbingObject*)GetScene()->GetGameObject(other);
			XMFLOAT3 size = co->GetSize();

			if (size.x > size.z)
				m_currentClimbNormal = { 0,0,1 };
			else
				m_currentClimbNormal = { 1,0,0 };

			Climb();
			m_enterCollideClimb = true;
			m_timerBeforeClimb.Pause();
		}
	}

}

void Player::OnDestroy()
{
	GetComponent<MeshRendererComponent>().DeleteMeshInstance();
}

void Player::CheckStates(float dt)
{
	if (m_stateManager.IsActiveState(States::_IDLE) == true)
		m_speed = PLAYER_SPEED * dt;

	if (m_stateManager.IsActiveState(States::_WALK) == true)
		m_speed = PLAYER_SPEED * dt;

	if (m_stateManager.IsActiveState(States::_RUN) == true)
	{
		m_speed = PLAYER_RUN_SPEED * dt;

		if (m_pursuitMode)
			m_sprintLimit = PLAYER_RUN_LIMIT;
		else
			m_sprintLimit -= dt;

		mp_sprintBar->UpdateBar(m_sprintLimit);
		if (m_sprintLimit <= 0.f)
		{
			m_stateManager.DesactivateState(States::_RUN);
			m_stateManager.ActivateState(States::_WALK);
		}
	}
	else
	{
		m_sprintLimit += dt;
		mp_sprintBar->UpdateBar(m_sprintLimit);
		if (m_sprintLimit >= PLAYER_RUN_LIMIT)
			m_sprintLimit = PLAYER_RUN_LIMIT;
	}

	if (m_stateManager.IsActiveState(States::_JUMP) == true)
	{
	}
	if (m_stateManager.IsActiveState(States::_CROUCH) == true)
	{
		m_speed = PLAYER_SPEED * dt;
	}
	if (m_stateManager.IsActiveState(States::_SLIDE) == true)
	{
		m_slideTimer.Resume();
		m_slideTimer.Update(dt);
		if (m_slideTimer.IsTargetReached() == true)
		{
			m_slideTimer.ResetProgress();
			m_slideTimer.Pause();

			m_stateManager.DesactivateState(States::_SLIDE);
			m_stateManager.DesactivateState(States::_RUN);
			m_stateManager.ActivateState(States::_CROUCH);

			m_doSlide = true;
		}
	}
	if (m_stateManager.IsActiveState(States::_CLIMB) == true)
	{
		m_speed = (PLAYER_SPEED * 0.5f) * dt;
		if (m_isClimbing == false)
		{
			DownClimb();
		}
	}
	if (m_stateManager.IsActiveState(States::_DEATH) == true)
	{

	}

	//Time Before Slide Again
	if (m_doSlide == true)
	{
		m_timeBeforeSlide.Resume();
		m_timeBeforeSlide.Update(dt);
		if (m_timeBeforeSlide.IsTargetReached() == true)
		{
			m_timeBeforeSlide.ResetProgress();
			m_timeBeforeSlide.Pause();

			m_doSlide = false;
		}
	}
}

void Player::HandleInputs()
{
	m_localMoveDir = { 0, 0, 0 };

	GamePad* p_0 = Input::GetGamePad(0);
	HandleCamera(p_0);

	if (m_stateManager.IsActiveState(States::_HIDE) == true)
	{
		if (Input::IsKeyDown('E'))
		{
			SwitchHideState();
		}
		return;
	}	

	//-----WALK-----//
	if (m_stateManager.IsActiveState(States::_CLIMB) == false)
	{
		if (Input::IsKey(GameKeybinds::GetKey(KeyAction::_MoveUp)))
			m_localMoveDir.z += 1;

		if (Input::IsKey(GameKeybinds::GetKey(KeyAction::_MoveDown)))
			m_localMoveDir.z -= 1;
	}

	if (Input::IsKey(GameKeybinds::GetKey(KeyAction::_MoveRight)))
		m_localMoveDir.x += 1;
	if (Input::IsKey(GameKeybinds::GetKey(KeyAction::_MoveLeft)))
		m_localMoveDir.x -= 1;
	
	if (p_0->IsConnected())
		m_localMoveDir = { p_0->GetLeftStickX(), 0.f, p_0->GetLeftStickY() };

	Move(m_speed);

	//IDLE
	if (m_localMoveDir.x == m_localMoveDir.z && m_localMoveDir.z == 0)
		m_stateManager.SetExclusiveActiveState(States::_IDLE);

	//JUMP
	if (Input::IsKeyDown(GameKeybinds::GetKey(KeyAction::_Jump)) || p_0->IsButtonDown(GameKeybinds::GetButton(KeyAction::_Jump)))
	{
		if(m_isGrounded)
			Jump(4.f);
	}

	//RUN
	if (Input::IsKeyDown(GameKeybinds::GetKey(KeyAction::_Sprint)) || p_0->IsButtonDown(GameKeybinds::GetButton(KeyAction::_Sprint)))
	{
		m_stateManager.ToggleState(States::_RUN);
		m_stateManager.DesactivateState(States::_WALK);

		if (m_sprintLimit <= 0.f)
			m_stateManager.DesactivateState(States::_RUN);
	}

	//CROUCH
	if (Input::IsKeyDown(GameKeybinds::GetKey(KeyAction::_Crouch)) || p_0->IsButtonDown(GameKeybinds::GetButton(KeyAction::_Crouch)))
	{
		Crouch();
	}
	else if (Input::IsKeyUp(GameKeybinds::GetKey(KeyAction::_Crouch)) || p_0->IsButtonUp(GameKeybinds::GetButton(KeyAction::_Crouch)))
	{
		StandUp();
	}

	//FLASHLIGHT
	if (Input::IsKeyDown(GameKeybinds::GetKey(KeyAction::_ToggleLight)) || p_0->IsButtonDown(GameKeybinds::GetButton(KeyAction::_ToggleLight)))
	{
		if(m_hasFlashlight)
			mp_flashlight->SwitchLight();
	}

	//RELOAD
	if (Input::IsKeyDown(GameKeybinds::GetKey(KeyAction::_ReloadLight)) || p_0->IsButtonDown(GameKeybinds::GetButton(KeyAction::_ReloadLight)))
	{
		if (m_hasFlashlight)
		{
			if (m_batteryCount > 0)
			{
				DecreaseBatteryCount();
				mp_flashlight->StartReplaceTimer();
			}
		}
	}

	if (m_stateManager.IsActiveState(States::_CLIMB) == true)
	{
		if (Input::IsKeyDown(GameKeybinds::GetKey(KeyAction::_Jump)) || p_0->IsButtonDown(GameKeybinds::GetButton(KeyAction::_Jump)) && m_isGrounded == true)
		{
			DownClimb();
		}
	}
}

void Player::HandleCamera(GamePad* p_0)
{
	App* app = App::Get();

	SceneManager& sm = app->GetSceneManager();

	float dt = app->GetChrono().GetScaledDeltaTime();

	GameObject* appCamera = app->GetCamera();

	XMFLOAT3 pos = mp_transform->GetWorldPosition();
	appCamera->mp_transform->SetWorldPosition(pos);
	UpdateChilds();

	if (m_isPaused == false)
	{
		Input::HideMouse();

		Window& window = app->GetWindow();
		XMFLOAT2 size = XMFLOAT2((float)window.m_clientWidth, (float)window.m_clientHeight);
		XMFLOAT2 centerSize = XMFLOAT2(size.x * 0.5f, size.y * 0.5f);
		Input::UpdateMouseDelta();

		XMFLOAT2 posMouse = Input::GetMousePostion();
		XMFLOAT2 delta = Input::GetMouseDelta();
		Input::SetMousePosition(centerSize);

		const GeneralData& dat = GeneralConfigs::GetDatas();
		const float sensivity = dat.m_playerSensivity * dat.m_sensivityScale;

		if (p_0->IsConnected() == false)
		{
			m_camYPR.x += (int)delta.x * sensivity;
			m_camYPR.y += (int)delta.y * sensivity;
		}
		else
		{
			m_camYPR.x += p_0->GetRightStickX() * sensivity * 5.f;
			m_camYPR.y += -p_0->GetRightStickY() * sensivity * 5.f;
		}

		if (m_stateManager.IsActiveState(States::_CLIMB) == true)
		{
			m_camYPR.y = std::clamp(m_camYPR.y, -XM_PIDIV2, XM_PIDIV2);
			m_camYPR.x = std::clamp(m_camYPR.x, -XM_PIDIV2, XM_PIDIV2);
		}
		else
		{
			m_camYPR.y = std::clamp(m_camYPR.y, -XM_PIDIV2, XM_PIDIV2);
		}
		mp_transform->SetWorldYPR(m_camYPR.x, 0, 0);
		appCamera->mp_transform->SetWorldYPR(m_camYPR.x, m_camYPR.y, 0);
	}
	else
		Input::ShowMouse();
}

void Player::SwitchHideState()
{
	if (m_stateManager.IsActiveState(States::_HIDE) == false)
	{
		m_stateManager.SetExclusiveActiveState(States::_HIDE);

		Monster* p_monster = (Monster*)GetScene()->GetFirstGameObjectWithTag((int)TagObject::_Monster);

		if (p_monster == nullptr)
			return;

		if(p_monster->m_stateManager.IsActiveState(Monster::States::_PURSUIT))
			return;

		if (p_monster->m_stateManager.IsActiveState(Monster::States::_FOLLOW))
			p_monster->ToPatrol();
	}
		
	m_stateManager.DesactivateState(States::_HIDE);
}

void Player::Move(float speed)
{
	if (m_localMoveDir.x == 0 && m_localMoveDir.z == 0)
		return;

	GetComponent<PhysicComponent>().ResetVelocity(true, false, true);
	m_stateManager.DesactivateState(States::_IDLE);

	if(m_stateManager.IsActiveState(States::_RUN) == false)
		m_stateManager.ActivateState(States::_WALK);

	XMVECTOR localMove = XMLoadFloat3(&m_localMoveDir);
	localMove = XMVector3Normalize(localMove);

	if (m_stateManager.IsActiveState(States::_CLIMB) == true)
	{
		XMVECTOR wallNormal = XMVectorSet(m_currentClimbNormal.x, m_currentClimbNormal.y, m_currentClimbNormal.z,0);
		XMVECTOR worldUp = XMVectorSet(0, 1, 0, 0);

		XMVECTOR ledgeSideDir = XMVector3Cross(wallNormal, worldUp);
		ledgeSideDir = XMVector3Normalize(ledgeSideDir);

		float horizontalInput = XMVectorGetX(localMove); 
		XMVECTOR moveDir = XMVectorScale(ledgeSideDir, horizontalInput);

		XMFLOAT3 t = {};
		XMStoreFloat3(&t, XMVectorScale(moveDir, speed));
		mp_transform->TranslateWorld(t);
	}
	else
	{
		XMFLOAT3 f = mp_transform->GetLocalAxis(_Forward);
		XMVECTOR forward = XMLoadFloat3(&f);

		XMFLOAT3 r = mp_transform->GetLocalAxis(_Right);
		XMVECTOR right = XMLoadFloat3(&r);

		XMVECTOR moveDir = XMVectorScale(forward, XMVectorGetZ(localMove)) + XMVectorScale(right, XMVectorGetX(localMove));
		moveDir = XMVector3Normalize(moveDir);

		XMFLOAT3 t = {};
		XMStoreFloat3(&t, XMVectorScale(moveDir, speed));
		mp_transform->TranslateWorld(t);
	}
	
	
}

void Player::Jump(float _impulse)
{
	m_stateManager.ActivateState(States::_JUMP);
	PhysicComponent& physicComp = GetComponent<PhysicComponent>();
	physicComp.AddImpulse({ 0, _impulse, 0 });
	m_isGrounded = false;
}

void Player::Crouch()
{
	if (m_stateManager.IsActiveState(States::_RUN) == true && m_doSlide == false && m_isGrounded == true)
	{
		m_stateManager.ActivateState(States::_SLIDE);
	}
	else
	{
		m_stateManager.ActivateState(States::_CROUCH);
	}

	XMFLOAT3 pos = mp_transform->GetWorldPosition();
	pos.y -= m_size.y * 0.25f;

	mp_transform->SetWorldPosition(pos);
	mp_transform->SetWorldScale({1, 0.5f, 1});
}

void Player::StandUp()
{
	if (m_stateManager.IsActiveState(States::_SLIDE) == true)
		m_stateManager.DesactivateState(States::_SLIDE);
	if (m_stateManager.IsActiveState(States::_CROUCH) == true)
		m_stateManager.DesactivateState(States::_CROUCH);

	XMFLOAT3 pos = mp_transform->GetWorldPosition();
	pos.y += m_size.y * 0.25f;

	mp_transform->SetWorldPosition(pos);
	mp_transform->SetWorldScale({1, 1, 1});
}

void Player::SetActiveUI(bool state)
{
	mp_flashlight->SetActiveUI(state);
	mp_sprintBar->SetActiveUI(state);
}

Inventory& Player::GetInventory()
{
	return m_inventory;
}

void Player::SetPursuitMode(bool state)
{
	m_pursuitMode = state;
}

void Player::SetHasFlashlight(bool state)
{
	m_hasFlashlight = state;

	if (CHRONO.IsFrozen())
		return;

	mp_flashlight->SetActiveUI(state);
	mp_flashlight->SetActive(state);
}

const bool Player::HasFlashlight() const
{
	return m_hasFlashlight;
}

void Player::Climb()
{
	m_stateManager.SetExclusiveActiveState(States::_CLIMB);
	GetComponent<PhysicComponent>().m_useGravity = false;
	mp_transform->SetWorldPosition({ mp_transform->GetWorldPosition().x
		, mp_transform->GetWorldPosition().y, mp_transform->GetWorldPosition().z });

	//Rotate Camera
	/*App* app = App::Get();
	GameObject* appCamera = app->GetCamera();
	XMVECTOR vectRotation = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), XM_PIDIV6);
	XMFLOAT4 rotation;
	XMStoreFloat4(&rotation, vectRotation);
	appCamera->mp_transform->SetWorldRotation(rotation);*/

	
}

void Player::DownClimb()
{
	m_stateManager.DesactivateState(States::_CLIMB);
	GetComponent<PhysicComponent>().m_useGravity = true;
	m_enterCollideClimb = false;
	m_timerBeforeClimb.ResetProgress();
	m_timerBeforeClimb.Resume();
	//mp_transform->SetWorldPosition({ mp_transform->GetWorldPosition().x + 0.25f
	//	, mp_transform->GetWorldPosition().y, mp_transform->GetWorldPosition().z });
	/*App* app = App::Get();
	GameObject* appCamera = app->GetCamera();
	XMVECTOR vectRotation = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), -XM_PIDIV6);
	XMFLOAT4 rotation;
	XMStoreFloat4(&rotation, vectRotation);
	appCamera->mp_transform->SetWorldRotation(rotation);*/
}

const int Player::GetBatteryCount() const
{
	return m_batteryCount;
}

void Player::IncreaseBatteryCount()
{
	m_batteryCount++;
}

void Player::DecreaseBatteryCount()
{
	m_batteryCount--;
	if (m_batteryCount <= 0)
		m_batteryCount = 0;
}

void Player::UpdateChilds()
{
	for (TransformComponent* child : mp_transform->GetChildren())
	{
		XMFLOAT3 pos = mp_transform->GetWorldPosition();

		/*	XMFLOAT3 forward = mp_transform->GetLocalAxis(_Forward);
			float offset = 0.f;

			pos.x += forward.x * offset;
			pos.y += forward.y * offset;
			pos.z += forward.z * offset;*/

		child->SetWorldPosition(pos);

		XMFLOAT4 rot = APP->GetCamera()->mp_transform->GetWorldRotation();
		child->SetWorldRotation(rot);
	}

	if (mp_ambientLight)
	{
		XMFLOAT3 pos = mp_transform->GetWorldPosition();
		mp_ambientLight->mp_transform->SetWorldPosition(pos);
	}
}

void Player::HandlePausing()
{
	if (m_inventory.IsOpened())
		return;

	int key = GameKeybinds::GetKey(KeyAction::_Pause);
	KeyButton button = GameKeybinds::GetButton(KeyAction::_Pause);

	if (Input::IsKeyDown(key) || Input::GetGamePad(0)->IsButtonDown(button))
	{
		if (SCENE_MANAGER.IsSceneActive((int)TagScene::_Options))
			return;

		m_isPaused = !m_isPaused;
		SetActiveUI(!m_isPaused);

		if (m_isPaused)
		{
			AudioManager::PauseCurrentMusic();
			Input::ShowMouse();
		}
		else
		{
			AudioManager::ResumeCurrentMusic();
			Input::HideMouse();

			Input::SetMousePosition(WINDOW->GetCenter());
		}

		SCENE_MANAGER.SetActiveScene((int)TagScene::_Pause, m_isPaused);
		CHRONO.SetFreezeState(m_isPaused);
	}
}

void Player::HandleInventory()
{
	if (m_isPaused)
		return;

	if (Input::IsKeyDown(GameKeybinds::GetKey(KeyAction::_Inventory)) || Input::GetGamePad(0)->IsButtonDown(GameKeybinds::GetButton(KeyAction::_Inventory)))
	{
		m_inventory.SwitchInventory();
		SetActiveUI(!m_inventory.IsOpened());
	}
}
