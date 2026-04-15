#pragma once
#include "PathFinding.h"

namespace nam
{
	struct TransformComponent;

	struct PathFindingComponent
	{
		Grid* mp_grid;
		int m_id = -1;

		Vector<Node*> m_path;
		Node* mp_position = nullptr;
		Node* mp_posToGo = nullptr;

		PathFinding m_pathfinder;
		bool m_isInPatrol = false;
		bool m_isPathFind = false;
		bool m_isFollowingPlayer = false;
		float m_pathFindingCD = 0.f;

		void InitPathfinding(Grid* grid,int id);
		void setPatrol(bool choice);

		void StartNewCheckpoint();
		void SetPosToGo(Node* nodeToGo);
		XMFLOAT3& SetStartPosition(size_t floor, size_t col, size_t row);
		XMFLOAT3& SetStartPosition(XMFLOAT3 pos);
		void SetPosition(size_t floor , size_t col ,size_t row);
		void GoToNextPath();
		bool SetNewPosition();
		bool IsArrived(TransformComponent& transform);
		void LookToCheckPoint(TransformComponent& transform);
	};
}
