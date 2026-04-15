#pragma once
#include <Scene.h>

class UIObject;
class Collectible;
class Button;

class InventoryScene : public Scene
{
	bool m_isTriggerCheck = false;

	bool m_isChecking = false;

	UnMap<std::string, std::pair<Button*, Button*>> m_buttonsList;
	UnMap<std::string, UIObject*> m_displayList;
	UnMap<std::string, std::pair<Collectible*, u8>> m_items;

	UIObject* mp_sprite = nullptr;
	UIObject* mp_description = nullptr;

	bool m_refreshDisplay = true;
public:
	void Init() override;
	void Start() override;
	void Update() override;
	void End() override;

	void AddItem(Collectible* c);

	void UseItem(std::string itemName);
	void CheckItem(std::string itemName);

	bool HasItem(std::string itemName) const;

	void SetActiveCheck(bool state, std::string desc = " ");

	const UnMap<std::string, std::pair<Collectible*, u8>>& GetItems() const;

	void FillInventoryFromFile();
	void ResetInventory();

private:
	void LoadAssets();

};