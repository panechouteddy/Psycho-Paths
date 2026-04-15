#pragma once
#include "ItemsList.h"

class Collectible;

class Inventory
{
private: 
	bool m_isOpened = false;

public:
	void SwitchInventory();
	bool HasItem(std::string name);
	void UseItem(std::string name);

	const bool IsOpened() const;

	const UnMap<std::string, std::pair<Collectible*, u8>>& GetItems() const;

	void FillInventoryFromFile();
	void ResetInventory();
};



