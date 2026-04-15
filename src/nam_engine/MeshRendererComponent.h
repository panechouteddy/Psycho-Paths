#pragma once
#include <Mesh.h>

namespace nam
{
	struct Mesh;

	struct MeshRendererComponent
	{
		Mesh* mp_mesh = nullptr;

	public:
		//Create a new Mesh in the component
		Mesh* CreateMeshInstance();

		//Using a existing Mesh
		void SetMeshInstance(Mesh* p_mesh);

		//Delete the Mesh member
		void DeleteMeshInstance();
	};
}
