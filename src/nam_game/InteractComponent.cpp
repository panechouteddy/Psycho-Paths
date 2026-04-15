#include "pch.h"
#include "InteractComponent.h"
#include "InventoryScene.h"

namespace nam
{
	void InteractComponent::InitInteract(std::function<void()> onTrigger)
	{
		OnTrigger = onTrigger;
	}
}