#pragma once
#include "ISystem.h"

class UIObject;

namespace nam
{
	class InteractorSystem : public ISystem
	{
		bool m_objectDetected = false;
		UIObject* mp_indicator = nullptr;

	public:
		void InitTextInteract();
		void SetPositiontoInteract(TransformComponent& tc);
		void Update(Ecs& ecs) override;
		bool IsInTheDetectionCone(u32 e ,TransformComponent& tc);
	};
}

