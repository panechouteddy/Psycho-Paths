#include "pch.h"
#include "MeshRendererComponent.h"
#include <RenderManager.h>

namespace nam
{ 
	Mesh* MeshRendererComponent::CreateMeshInstance()
	{
		mp_mesh = RENDER_MANAGER->GetRenderItemManager().CreateRenderItem<Mesh>();

		return mp_mesh;
	}

	void MeshRendererComponent::SetMeshInstance(Mesh* p_mesh)
	{
		if (p_mesh == nullptr)
		{
			_ASSERT(false && "You setted a nullptr Mesh");
			return;
		}
		
		if (p_mesh->GetID() == INVALID_ID)
		{
			_ASSERT(false && "YOU CAN'T CREATE RAW MESH INSTANCES, YOU NEED TO CREATE A MESH\nWITH THE RenderItemManager class : Use app->CreateEmptyMesh()");
			return;
		}

		mp_mesh = p_mesh;
	}

	void MeshRendererComponent::DeleteMeshInstance()
	{
		RENDER_MANAGER->GetRenderItemManager().RemoveRenderItem(mp_mesh);
	}
}
