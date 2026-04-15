#pragma once
#include <GameObject.h>

class Collectible : public GameObject
{
public:
	bool m_canUse = false;
	std::string m_description = "";

public:
	void SetProperties(std::string name, std::string description, bool canUse);
	virtual void OnUse() { };
	virtual void OnCheck() { };
};

