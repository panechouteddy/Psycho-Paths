#include "pch.h"
#include "PathFindingComponent.h"


namespace nam
{
	void PathFindingComponent::InitPathfinding(Grid* grid, int id)
	{
		mp_grid = grid;
		m_id = id;
	}

	void PathFindingComponent::setPatrol(bool choice)
	{
		m_isInPatrol = choice;
	}

	void PathFindingComponent::StartNewCheckpoint()
	{
		m_pathfinder.InitEndNode(mp_grid->GetCheckpoint());
		m_isPathFind = m_pathfinder.FindPath(mp_position);

		if (m_isPathFind)
		{
			m_path = m_pathfinder.GetPathNodes();
			GoToNextPath();

			m_pathFindingCD = 1.f;
		}
	}
	void PathFindingComponent::SetPosToGo(Node* nodeToGo)
	{

		m_pathfinder.InitEndNode(nodeToGo);

		m_isPathFind = m_pathfinder.FindPath(mp_position);

		if (m_isPathFind)
		{
			m_path = m_pathfinder.GetPathNodes();
			GoToNextPath();

			m_pathFindingCD = 1.f;
		}
	}
	XMFLOAT3& PathFindingComponent::SetStartPosition(size_t floor, size_t col, size_t row)
	{
		if (mp_position != nullptr)
		{
			Node* node = mp_grid->GetNodesFromID(m_id);

			if (node != nullptr)
			{
				node->m_id = 0;
				node->m_isWalkable = true;
			}
		}

		Node* n = mp_grid->GetNode(floor, col, row);
		n->m_id = m_id;
		mp_position = n;
		StartNewCheckpoint();

		return n->m_pos;
	}

	XMFLOAT3& PathFindingComponent::SetStartPosition(XMFLOAT3 pos)
	{
		if (mp_position != nullptr)
		{
			Node* node = mp_grid->GetNodesFromID(m_id);

			if (node != nullptr)
			{
				node->m_id = 0;
				node->m_isWalkable = true;
			}
		}

		Node* n = mp_grid->GetNearestNode(pos);
		n->m_id = m_id;
		mp_position = n;
		StartNewCheckpoint();

		return n->m_pos;
	}

	void PathFindingComponent::SetPosition(size_t floor, size_t col, size_t row)
	{
		mp_position = mp_grid->GetNode(floor, col, row);

	}

	void PathFindingComponent::GoToNextPath()
	{
		if (m_path.empty())
			return;
		mp_posToGo = m_path.back();
		m_path.pop_back();
	}

	bool PathFindingComponent::SetNewPosition()
	{
		if (!mp_grid->UpdateNodePosition(mp_position->m_data, mp_position, mp_posToGo))
			return false;
		else
		{
			mp_position = mp_posToGo;
			return true;
		}
	}

	bool PathFindingComponent::IsArrived(TransformComponent& transform)
	{
		if (mp_posToGo == nullptr)
			return false;

		XMFLOAT3 pos = transform.GetWorldPosition();
		pos.y = 0;
		return Maths::XMFLOAT3IsEqual(pos, mp_posToGo->m_pos, 0.10f);
	}

	void PathFindingComponent::LookToCheckPoint(TransformComponent& transform)
	{
		XMFLOAT3 dir = Maths::GetDirection(transform.GetWorldPosition(), mp_posToGo->m_pos);
		dir.y = 0;
		XMStoreFloat3(&dir, XMVector3Normalize(XMLoadFloat3(&dir)));

		transform.LookToWorld(dir);
	}



}