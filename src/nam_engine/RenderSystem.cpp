#include "pch.h"
#include "RenderSystem.h"

#include "MeshRendererComponent.h"
#include "SpriteRendererComponent.h"
#include "TextRendererComponent.h"

#include "TransformComponent.h"

#include "DataShader.h"
#include "RenderManager.h"
#include "App.h"

#include <nam_core/ContainerAssociative.h>

namespace nam
{
	struct Render2DItemData
	{
		ARenderItem* mp_item = nullptr;
		int m_drawLayer = 0;
		TransformComponent* mp_transform = nullptr;
	};

	struct DrawLayerComparator
	{
		bool operator()(const Render2DItemData& p_a, const Render2DItemData& p_b) const
		{
			return p_a.m_drawLayer < p_b.m_drawLayer;
		}
	};
}

namespace nam
{
	void RenderSystem::Update(Ecs& ecs)
	{
		int iTex3D = 0;
		int iTex2D = 0;

		RenderManager* p_renderManager = RENDER_MANAGER;

		TransformComponent* tc = nullptr;
		CameraComponent* cc = nullptr;

		ecs.ForEach<CameraComponent, TransformComponent>(
			[&](u32 e, CameraComponent& c, TransformComponent& t) {
				if (tc != nullptr || cc != nullptr)
					return;

				tc = &t;
				cc = &c;
			}
		);

		//3D Render

		if (p_renderManager->IsFirstDraw() == false)
		{
			ecs.ForEach<TransformComponent, MeshRendererComponent>([&](uint32_t entity, TransformComponent& transform, MeshRendererComponent& meshRenderer)
			{		
				transform.UpdateWorldData();

				Mesh* p_mesh = meshRenderer.mp_mesh; 
				
				if (p_mesh->IsVisible() == false)
					return;

				XMFLOAT3 sca = transform.GetWorldScale();

				float maxScale = max(max(sca.x, sca.y), sca.z);

				auto obj = SCENE_MANAGER.GetScene(0)->GetGameObject(entity);

				if (cc->IsVisibleInRadius(transform.GetWorldPosition(), p_mesh->GetBoundingSphereRadius() * maxScale) == false)
					return;

				CBufferObj3D objConstants;

				if (p_mesh->GetAnimModel() == nullptr)
					objConstants.m_isSkinned = false;
				else
					objConstants.m_isSkinned = true;

				objConstants.m_world = transform.GetWorldMatrixTranspose();
				objConstants.m_invWorld = transform.GetInverseWorldMatrixTranspose();
				objConstants.m_hasTexture = p_mesh->IsUsingTexture();

				p_renderManager->GetConstantBuffersDatas()->mp_Obj3D_CB->CopyData(iTex3D, objConstants);
				iTex3D++;

				p_renderManager->AddToDraw(p_mesh);
			});
		}

		PrioQueue<Render2DItemData, Vector<Render2DItemData>, DrawLayerComparator> pq;
		Sprite* p_splashScreen = nullptr; //Store it to draw at the end

		//2D Render
		ecs.ForEach<TransformComponent, SpriteRendererComponent>([&](uint32_t entity, TransformComponent& transform, SpriteRendererComponent& spriteRenderer)
		{
			transform.UpdateWorldData();

			Render2DItemData spriteData;
			spriteData.mp_item = spriteRenderer.mp_sprite;
			spriteData.mp_transform = &transform;
			spriteData.m_drawLayer = spriteRenderer.mp_sprite->m_drawLayer;

			pq.push(spriteData);
		});

		//Text
		ecs.ForEach<TransformComponent, TextRendererComponent>([&](uint32_t entity, TransformComponent& transform, TextRendererComponent& textRenderer)
			{
				transform.UpdateWorldData();

				Render2DItemData textData;
				textData.mp_item = textRenderer.mp_text;
				textData.mp_transform = &transform;
				textData.m_drawLayer = textRenderer.mp_text->m_drawLayer;

				pq.push(textData);
			});

		//Draw 2D in order
		while (pq.empty() == false)
		{
			Render2DItemData sd = pq.top();
			pq.pop();

			ARenderItem* p_item = sd.mp_item;

			if (p_item->IsVisible() == false)
				return;

			TransformComponent* p_transform = sd.mp_transform;

			CBufferSprite2D item2DConstants;

			item2DConstants.m_world = p_transform->GetWorldMatrixTranspose();
			item2DConstants.m_hasTexture = p_item->IsUsingTexture();

			p_renderManager->GetConstantBuffersDatas()->mp_Sprite2D_CB->CopyData(iTex2D, item2DConstants);
			iTex2D++;

			p_renderManager->AddToDraw(p_item);
		}
	}
}
