#include "pch.h"
#include "PathFindingSystem.h"



namespace nam
{
	void PathFindingSystem::Update(Ecs& ecs)
	{
		ecs.ForEach<PathFindingComponent, TransformComponent>(
			[&](u32 e, PathFindingComponent& c, TransformComponent& t)
			{
				if (!c.m_isInPatrol)
					return;

				if (c.m_path.empty())
				{
					if (!c.m_isFollowingPlayer)
						c.StartNewCheckpoint();
				}

				if (c.m_isPathFind)
				{
					if (!c.IsArrived(t))
					{
						c.LookToCheckPoint(t);
						t.MoveWorld(2 * CHRONO.GetScaledDeltaTime(), LocalAxis::_Forward);

					}
					else if (!c.SetNewPosition())
						c.m_pathfinder.FindPath(c.mp_position);
					else
						c.GoToNextPath();
				}
			
				

				c.mp_grid->ResetGrid();
				c.m_pathFindingCD -= CHRONO.GetScaledDeltaTime();
			}
		);
	}
}