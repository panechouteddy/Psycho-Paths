#include "pch.h"
#include "UISystem.h"
#include "ButtonComponent.h"
#include "TransformComponent.h"
#include "SpriteRendererComponent.h"
#include "Input.h"

using namespace DirectX;

namespace nam
{
	void UISystem::Update(Ecs& ecs)
	{
		XMFLOAT2 mousePos = Input::GetMousePostion();

		ecs.ForEach<ButtonComponent, SpriteRendererComponent, TransformComponent>([&](u32 entity, ButtonComponent& button, SpriteRendererComponent& sprite, TransformComponent& transform)
		{
			transform.UpdateWorldData();

			XMFLOAT3 worldPos = transform.GetWorldPosition();
			XMFLOAT3 worldScale = transform.GetWorldScale();
			XMFLOAT3 ypr = transform.GetWorldYPR();

			float halfWidth = sprite.mp_sprite->m_size.x * worldScale.x * 0.5f;
			float halfHeight = sprite.mp_sprite->m_size.y * worldScale.y * 0.5f;

			bool isHovered = false;

			float localX = mousePos.x - worldPos.x;
			float localY = mousePos.y - worldPos.y;

			float cosZ = cosf(ypr.z);
			float sinZ = sinf(ypr.z);

			float rotatedLocalX = localX * cosZ + localY * sinZ;
			float rotatedLocalY = -localX * sinZ + localY * cosZ;

			isHovered = (rotatedLocalX >= -halfWidth && rotatedLocalX <= halfWidth && rotatedLocalY >= -halfHeight && rotatedLocalY <= halfHeight);
			
			if (isHovered != button.m_isHovered)
			{
				button.m_isHovered = isHovered;
				if (isHovered && button.OnHovered)
					button.OnHovered();
				else if (!isHovered && button.OnLeft)
					button.OnLeft();
			}

			if (button.m_isHovered && button.OnClick && Input::IsKeyDown(VK_LBUTTON))
			{
				button.OnClick();
			}
		});
	}
}
