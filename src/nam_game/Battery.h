#pragma once
#include "GameObject.h"

#include "Collectible.h"

class Battery : public Collectible
{
public:
	void OnInit() override;
	void OnUse() override;
	void OnCheck() override {};
};

