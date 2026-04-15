#pragma once
#include <DirectXCollision.h>

namespace nam
{
    template<typename BoxType1, typename BoxType2>
    inline void ColliderSystem::CheckCollision(TransformComponent& transform1, TransformComponent& transform2, BoxType1& box1, BoxType2& box2, u32 entity1, u32 entity2, Vector<CollisionInfo>& collisions)
    {
        if (!box1.m_box.Intersects(box2.m_box))
            return;

        CollisionInfo collisionInfo = CalculateCollisionInfo(box1.m_box, box2.m_box);
        collisionInfo.m_entity1 = entity1;
        collisionInfo.m_entity2 = entity2;
        collisionInfo.m_transform1 = &transform1;
        collisionInfo.m_transform2 = &transform2;
        collisionInfo.OnCollision1 = box1.OnCollision;
        collisionInfo.OnCollision2 = box2.OnCollision;
        collisionInfo.m_noIntersectionPush1 = box1.m_noIntersectionPush;
        collisionInfo.m_noIntersectionPush2 = box2.m_noIntersectionPush;

        collisions.push_back(collisionInfo);
    }
}
