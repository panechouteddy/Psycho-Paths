#include "pch.h"
#include "DigiCodeObject.h"
#include "UIObject.h"
#include "InteractComponent.h"
#include "InventoryScene.h"
#include <regex>

void DigiCodeObject::OnInit()
{
	SetBehavior();

	Mesh* p_mesh = RENDER_ITEM_MANAGER.CreateRenderItem<Mesh>();
	p_mesh->BuildBox({ 0.05f, 0.05f, 0.05f }, { 0.5f, 0.5f, 0.5f, 1 });

	mp_codeInterface = GetScene()->CreateGameObject<UIObject>();

	for (int i = 0; i < 10; i++)
	{
		GameObject* go = GetScene()->CreateGameObject<GameObject>();

		go->m_name = std::to_string(i);

		MeshRendererComponent mrc;
		mrc.SetMeshInstance(p_mesh);
		go->AddComponent(mrc);

		InteractComponent ic;
		ic.m_indicator = std::to_string(i);
		ic.InitInteract([this, go, i]()
			{
				AudioManager::PlaySound("ButtonHover", 1.5f);
				this->m_CodeValues += std::to_string(i);
			});
		go->AddComponent(ic);

		go->AddBoxCollider((int)TagCollision::_Decor);
		m_buttons[i] = go;
	}
	mp_transform->SetWorldPosition({ 5,1,0 });

	AddBoxCollider((int)TagCollision::_Decor);

	XMFLOAT2 textPosition = WINDOW->GetCenter(true);
	textPosition.x += textPosition.x * 0.65f;
	textPosition.y -= textPosition.y * 0.75f;
	mp_codeInterface->CreateText(textPosition, " ", 1, { 1,1,1,0.75f });

	mp_codeInterface->SetActive(false);
}

void DigiCodeObject::OnStart()
{
	mp_codeInterface->SetActive(false);
	m_isActiveCode = true;
	m_CodeValues.clear();

	Mesh* p_digicodeMesh = RENDER_ITEM_MANAGER.CreateRenderItem<Mesh>();
	MeshRendererComponent mrc;

	if (m_digiCodeOnX)
	{
		p_digicodeMesh->SetTexture((int)TagTexture::_Floor);
		p_digicodeMesh->BuildBox({ 0.5f, 0.5f, 0.01f }, { 1, 1, 1, 1 });
		mrc.SetMeshInstance(p_digicodeMesh);
		AddComponent(mrc);
	}
	else
	{
		p_digicodeMesh->SetTexture((int)TagTexture::_Floor);
		p_digicodeMesh->BuildBox({ 0.01f, 0.5f, 0.5f }, { 1, 1, 1, 1 });
		mrc.SetMeshInstance(p_digicodeMesh);
		AddComponent(mrc);
	}

	XMFLOAT3 dim = { mrc.mp_mesh->m_extend.x * 2.f,mrc.mp_mesh->m_extend.y * 2.f,mrc.mp_mesh->m_extend.z * 2.f};
	XMFLOAT3 basePos = mp_transform->GetWorldPosition();

	float spacingX = dim.x * 0.30f;
	float spacingY = dim.y * 0.20f;
	float spacingZ = dim.z * 0.30f;

	float depthOffsetX = dim.x * 0.10f;
	float depthOffsetZ = dim.z * 0.10f;

	float startx =  dim.x * 0.6f;
	float starty = dim.y - dim.y * 0.7f;
	float startz = dim.z - dim.z * 0.40f;

	for (int i = 0; i < 3; i++)
	{
		for (int j = 1; j < 4; j++)
		{
			XMFLOAT3 pos = basePos;

			if (m_digiCodeOnX)
			{
				pos.x -= startx - j * spacingX;
				pos.y += starty - i * spacingY;
				pos.z += depthOffsetZ;
			}
			else
			{
				pos.x -= depthOffsetX;
				pos.y += starty - i * spacingY;
				pos.z += startz - j * spacingZ;
			}

			m_buttons[i * 3 + j]->mp_transform->SetWorldPosition(pos);
		}
		{
			XMFLOAT3 pos = basePos;

			if (m_digiCodeOnX)
			{
				pos.x += startx - 2 * spacingX;
				pos.y += starty - 3 * spacingY;
				pos.z += depthOffsetZ;
			}
			else
			{
				pos.x -= depthOffsetZ;
				pos.y += starty - 3 * spacingY;
				pos.z += startz - 2 * spacingZ;
			}

			m_buttons[0]->mp_transform->SetWorldPosition(pos);

		}
	}

	InitClosestDoor();
}

void DigiCodeObject::OnUpdate()
{
	if (!m_isActiveCode)
	{
		mp_codeInterface->SetActive(false);
			return;
	}
	else
		mp_codeInterface->SetActive(true);

	if (SCENE_MANAGER.IsSceneActive((int)TagScene::_Inventory) || SCENE_MANAGER.IsSceneActive((int)TagScene::_Pause))
	{
		mp_codeInterface->SetActive(false);
		return;
	}

	if (m_CodeValues.size() > m_Code.size())
		m_CodeValues.clear();

	if (m_CodeValues.size() == m_Code.size() )
	{
		if (m_CodeValues == m_Code)
		{
			AudioManager::PlaySound("SaveButton");
			m_isActiveCode = false;

			InteractComponent& ic = m_door->GetComponent<InteractComponent>();
			ic.InitInteract([this, &ic]()
				{
					m_door->CanOpen(true);
					m_door->ToggleDoor();
				});
		}
		else
			m_CodeValues.clear();
	}

	std::string codeValue = "Code : " + m_CodeValues;
	mp_codeInterface->mp_text->SetText(codeValue);
}

void DigiCodeObject::InitClosestDoor()
{
	Door* closestDoor = nullptr;
	float smallestSquaredDist = (float)INT_MAX;

	for (GameObject * go : GetScene()->GetAllGameObjects())
	{
		std::regex pattern("door");
		if (std::regex_search(go->m_name, pattern))
		{
			float dx = abs(mp_transform->GetWorldPosition().x - go->mp_transform->GetWorldPosition().x);
			float dz = abs(mp_transform->GetWorldPosition().z - go->mp_transform->GetWorldPosition().z);

			float squaredDist = dx * dx + dz * dz;

			if (squaredDist < smallestSquaredDist)
			{
				closestDoor = (Door*)go;
				smallestSquaredDist = squaredDist;
			}
		}
	}

	m_door = closestDoor;
}

void DigiCodeObject::InitCode(std::string code)
{
	m_Code = code;
}


