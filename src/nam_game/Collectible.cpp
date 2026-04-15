#include "pch.h"
#include "Collectible.h"

#include "InteractComponent.h"
#include "InventoryScene.h"

void Collectible::SetProperties(std::string name, std::string description, bool canUse)
{
	m_name = name;
	m_description = description;
	m_canUse = canUse;

	InteractComponent ic;
	ic.InitInteract([this]()
		{
			InventoryScene* p_is = (InventoryScene*)SCENE_MANAGER.GetScene((int)TagScene::_Inventory);
			p_is->AddItem(this);
			AudioManager::PlaySound("PickUp");
			SetActive(false);
		});

	AddComponent(ic);
	AddBoxCollider((int)TagCollision::_Decor);
}

