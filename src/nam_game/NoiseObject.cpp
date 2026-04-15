#include "pch.h"
#include "NoiseObject.h"

#include "NoiseComponent.h"

void NoiseObject::SetNoiseComponent(std::string soundName, float soundScale, float soundRange)
{
	if (!HasComponent<NoiseComponent>())
		AddComponent(NoiseComponent());

	NoiseComponent& nc = GetComponent<NoiseComponent>();

	nc.m_soundName = soundName;
	nc.m_soundScale = soundScale;
	nc.m_soundRange = soundRange;
	nc.m_activated = false;
}

void NoiseObject::OnCollision(u32 self, u32 other, const CollisionInfo& collisionInfo)
{
	GameObject* gameObject = GetScene()->GetGameObject(other);
	int tagOther = gameObject->GetTag();

	if (tagOther == (int)TagObject::_Player)
		TriggerNoise();
}

void NoiseObject::TriggerNoise()
{
	if (HasComponent<NoiseComponent>() == false)
		return;

	GetComponent<NoiseComponent>().m_activated = true;
}
