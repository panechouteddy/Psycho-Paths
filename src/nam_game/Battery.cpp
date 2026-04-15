#include "pch.h"
#include "Battery.h"
#include "InteractComponent.h"

#include "ItemsList.h"

void Battery::OnInit()
{
	SetProperties(ItemListName::Battery, ItemListDesc::Battery, true);
}

void Battery::OnUse()
{
	SetActive(false);
	Player* player = (Player*)SCENE_MANAGER.GetScene((int)TagScene::_Main)->GetFirstGameObjectWithTag((int)TagObject::_Player);

	if (player == nullptr)
		return;

	player->IncreaseBatteryCount();
}