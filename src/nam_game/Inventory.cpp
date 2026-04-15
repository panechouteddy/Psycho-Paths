#include "pch.h"
#include "Inventory.h"
#include "InventoryScene.h"
#include "UIObject.h"

#include "InteractComponent.h"

#include "Collectible.h"

void Inventory::SwitchInventory()
{
	m_isOpened =  !m_isOpened;
	if (m_isOpened == true)
	{
		CHRONO.SetFreezeState(true);
		SCENE_MANAGER.SetActiveScene((int)TagScene::_Inventory, true);
		Input::ShowMouse();
	}
	else
	{
		XMFLOAT2 middle = WINDOW->GetCenter();
		Input::SetMousePosition(middle);
		CHRONO.SetFreezeState(false);
		SCENE_MANAGER.SetActiveScene((int)TagScene::_Inventory, false);
	}
}

bool Inventory::HasItem(std::string name)
{
	InventoryScene* is = (InventoryScene*)SCENE_MANAGER.GetScene((int)TagScene::_Inventory);
	return is->HasItem(name);
}

void Inventory::UseItem(std::string name)
{
	InventoryScene* is = (InventoryScene*)SCENE_MANAGER.GetScene((int)TagScene::_Inventory);
	is->UseItem(name);
}

const bool Inventory::IsOpened() const
{
	return m_isOpened;
}

const UnMap<std::string, std::pair<Collectible*, u8>>& Inventory::GetItems() const
{
	InventoryScene* is = (InventoryScene*)SCENE_MANAGER.GetScene((int)TagScene::_Inventory);

	return is->GetItems();
}

void Inventory::FillInventoryFromFile()
{
	InventoryScene* is = (InventoryScene*)SCENE_MANAGER.GetScene((int)TagScene::_Inventory);
	is->FillInventoryFromFile();
}

void Inventory::ResetInventory()
{
	InventoryScene* is = (InventoryScene*)SCENE_MANAGER.GetScene((int)TagScene::_Inventory);
	is->ResetInventory();
}

