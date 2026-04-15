#include "pch.h"
#include "InventoryScene.h"

#include "Collectible.h"

#include "UIObject.h"

#include "Button.h"

#include "SaveManager.h"

#include "Battery.h"

void InventoryScene::Init()
{
	LoadAssets();

	XMFLOAT2 winCenter = WINDOW->GetCenter(true);
	XMFLOAT2 winSize = WINDOW->GetSize(true);

	UIObject* menu = CreateGameObject<UIObject>();
	menu->CreateSprite({ winSize.x * 0.5f, winSize.y * 0.5f }, { winSize.x, winSize.y }, { 0.5f ,0.5f, 0.5f, 0.75f }, (int)TagTexture::_Inventory);
	menu->mp_sprite->SetDrawLayer(3);

	UIObject* inventoryText = CreateGameObject<UIObject>();
	inventoryText->CreateText({ winSize.x * 0.5f, winSize.y * 0.075f }, "Inventaire", 1, { 1, 1, 1, 1 });
	inventoryText->mp_text->SetDrawLayer(2);

	mp_sprite = CreateGameObject<UIObject>();
	{
		mp_sprite->CreateSprite(winCenter, { winSize.x * 0.85f, winSize.y * 0.85f }, {1, 1, 1, 1}, (int)TagTexture::_Inventory);
		mp_sprite->mp_sprite->SetDrawLayer(0);
	}

	mp_description = CreateGameObject<UIObject>();
	{
		mp_description->CreateText(winCenter, "Aucune", 1, { 1, 1, 1, 1 });
		mp_description->mp_text->SetDrawLayer(-1);
	}
}

void InventoryScene::Start()
{
	m_refreshDisplay = true;
	m_isChecking = false;
	AudioManager::PlaySound("OpenInv");
	AudioManager::PauseCurrentMusic();

	StartGameObjects();

	mp_description->SetActive(false);
	mp_sprite->SetActive(false);
}

void InventoryScene::Update()
{
	if (m_isChecking)
	{
		if (m_isTriggerCheck)
		{
			m_isTriggerCheck = false;
			return;
		}

		if (Input::IsKeyDown(VK_LBUTTON) || Input::GetGamePad(0)->IsButtonDown(KeyButton::B))
			SetActiveCheck(false);

		return;
	}

	if (m_refreshDisplay == false)
		return;

	m_refreshDisplay = false;

	XMFLOAT2 winStartCenter = WINDOW->GetCenter(true);
	float offsetY = 0.f;

	XMFLOAT2 pos1 = {};
	pos1.x = winStartCenter.x * 0.5f;

	XMFLOAT2 pos2 = {};
	pos2.x = winStartCenter.x * 1.5f;

	for(auto& item : m_items)
	{
		pos1.y = winStartCenter.y * 0.3f + offsetY;
		pos2.y = pos1.y;

		XMFLOAT2 pos3 = pos2;
		pos3.x += 125.f;

		std::string name = item.first;
		std::string message = (name + " : X " + std::to_string(item.second.second));

		m_displayList[name]->SetUiPosition(pos1);
		m_displayList[name]->mp_text->SetText(message);
		m_buttonsList[name].first->SetUiPosition(pos2);

		if (Button* use = m_buttonsList[name].second)
			use->SetUiPosition(pos3);

		offsetY += 60.f;
	}
}

void InventoryScene::End()
{
	AudioManager::PlaySound("CloseInv");
	AudioManager::ResumeCurrentMusic();
}

void InventoryScene::AddItem(Collectible* c)
{
	m_refreshDisplay = true;

	std::string name = c->m_name;
	auto it = m_items.find(name);

	if (it == m_items.end()) // New item
	{
		XMFLOAT4 color = { 0.7f, 0.8f, 1.f, 0.15f };
		float txtSize = 0.6f;

		m_items[name].first = c;
		m_items[name].second = 1;

		UIObject* itemText = CreateGameObject<UIObject>();
		itemText->CreateText({ 0, 0 }, name, 1, { 1, 1, 1, 1 });
		itemText->mp_text->SetDrawLayer(2);
		m_displayList[name] = itemText;

		Button* check = CreateGameObject<Button>();
		check->SetProperties({ 0, 0 }, { 100, 50 }, color, -1, -1,
			[this, name]() { if (m_isChecking)return; CheckItem(name); });
		check->AddText("Check", { txtSize, txtSize }, {1, 1, 1, 1}, 2);

		m_buttonsList[name].first = check;

		if (c->m_canUse)
		{
			Button* use = CreateGameObject<Button>();
			use->SetProperties({ 0, 0 }, { 100, 50 }, color, -1, -1,
				[this, name]() { if (m_isChecking)return; UseItem(name); });
			use->AddText("Utiliser", { txtSize, txtSize }, { 1, 1, 1, 1 }, 2);

			m_buttonsList[name].second = use;
		}
		else
		{
			m_buttonsList[name].second = nullptr;
		}
	}
	else // Existing item
	{
		m_items[name].second += 1;
	}
}

void InventoryScene::UseItem(std::string itemName)
{
	m_refreshDisplay = true;

	auto it = m_items.find(itemName);

	if (it == m_items.end())
		return;

	u8& currentAmount = it->second.second;

	if (currentAmount <= 0)
		return;

	it->second.first->OnUse();
	currentAmount -= 1;

	if (currentAmount <= 0)
	{
		m_items.erase(itemName);

		m_displayList[itemName]->Destroy();
		m_displayList.erase(itemName);

		m_buttonsList[itemName].first->Destroy();
		if(Button* use = m_buttonsList[itemName].second)
			use->Destroy();

		m_buttonsList.erase(itemName);
	}

	AudioManager::PlaySound("OnUse");

	return;
}

void InventoryScene::CheckItem(std::string itemName)
{
	auto it = m_items.find(itemName);

	if (it == m_items.end())
		return;

	m_isChecking = true;
	it->second.first->OnCheck();

	SetActiveCheck(true, it->second.first->m_description);
}

bool InventoryScene::HasItem(std::string itemName) const
{
	auto it = m_items.find(itemName);

	if (it == m_items.end())
		return false;

	//If having more than 0 amount of this item
	return it->second.second > 0;
}

void InventoryScene::SetActiveCheck(bool state, std::string desc)
{
	XMFLOAT2 winCenter = WINDOW->GetCenter(true);

	m_isTriggerCheck = true;
	m_isChecking = state;

	mp_sprite->SetActive(state);
	mp_description->SetActive(state);
	mp_description->mp_text->SetText(desc);
	mp_description->SetUiPosition(winCenter);
}

const UnMap<std::string, std::pair<Collectible*, u8>>& InventoryScene::GetItems() const
{
	return m_items;
}

void InventoryScene::FillInventoryFromFile()
{
	ResetInventory();

	InventoryDatas& inventory = SaveManager::GetDatas().inventory;

	for (auto& item : inventory.datas)
	{
		std::string name = item.first;
		u8 amount = item.second;

		Collectible* p_instance = nullptr;

		if (name == ItemListName::Battery)
		{
			Battery* p_battery = SCENE_MANAGER.GetScene((int)TagScene::_Main)->CreateGameObject<Battery>();
			p_instance = p_battery;
		}
		else if (name == ItemListName::FirstKey)
		{
			Collectible* o = SCENE_MANAGER.GetScene((int)TagScene::_Main)->CreateGameObject<Collectible>();
			o->SetProperties(ItemListName::FirstKey, ItemListDesc::FirstKey, false);
			p_instance = o;
		}

		if (p_instance == nullptr)
			continue;

		p_instance->SetActive(false);
		for (size_t i = 0; i < amount; i++)
		{
			AddItem(p_instance);
		}

		int a = 0;
	}
}

void InventoryScene::ResetInventory()
{
	m_refreshDisplay = true;

	for (auto& item : m_items)
	{
		std::string name = item.first;

		m_displayList[name]->Destroy();
		m_displayList.erase(name);

		m_buttonsList[name].first->Destroy();
		if (Button* use = m_buttonsList[name].second)
			use->Destroy();

		m_buttonsList.erase(name);
	}

	for (auto& item : m_items)
	{
		item.second.first->Destroy();
	}

	m_items.clear();
}

void InventoryScene::LoadAssets()
{
	APP->LoadTexture(L"../../res/Textures/UI/inventoryUI.dds", (int)TagTexture::_Inventory, false);

	AudioManager::LoadSound("PickUp", "../../res/Sfx/pickup.wav", 1.5f);
	AudioManager::LoadSound("OpenInv", "../../res/Sfx/openInv.wav", 0.75f);
	AudioManager::LoadSound("CloseInv", "../../res/Sfx/closeInv.wav", 0.75f);
	AudioManager::LoadSound("OnUse", "../../res/Sfx/onUse.wav", 1.f);
}