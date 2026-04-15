#include "pch.h"
#include "ColliderSystem.h"
#include "TransformComponent.h"
#include "MeshRendererComponent.h"
#include "BoxColliderComponent.h"
#include "SphereColliderComponent.h"
#include "PhysicComponent.h"

using namespace DirectX;

namespace nam
{
	void ColliderSystem::Update(Ecs& ecs)
	{
		m_updateId++;
        Vector<CollisionInfo> collisions;

        m_spatialHash.Clear();
        ecs.ForEach<BoxColliderComponent, TransformComponent>(
            [&](u32 e, BoxColliderComponent& b, TransformComponent& t) {
                t.UpdateWorldData();

                if (ecs.HasComponent<MeshRendererComponent>(e))
                    b.UpdateObbBox(t, ecs.GetComponent<MeshRendererComponent>(e), m_updateId);
                else
                    b.UpdateObbBox(t, m_updateId);

                m_spatialHash.Insert(e, b.m_box);
            }
        );

        ecs.ForEach<SphereColliderComponent, TransformComponent, MeshRendererComponent>(
            [&](u32 e, SphereColliderComponent& s, TransformComponent& t, MeshRendererComponent& m) {
                t.UpdateWorldData();

                s.UpdateSphereBox(t, m, m_updateId);
                m_spatialHash.Insert(e, s.m_box);
            }
        );

        ecs.ForEach<BoxColliderComponent, TransformComponent, MeshRendererComponent>(
            [&](u32 e1, BoxColliderComponent& b1, TransformComponent& t1, MeshRendererComponent& m) {
                Vector<u32> nearby;
                m_spatialHash.GetNearby(e1, b1.m_box, nearby);

                for (u32 e2 : nearby)
                {
                    if (e1 >= e2) continue; 

                    if (ecs.HasComponent<BoxColliderComponent>(e2)) {
                        BoxColliderComponent& b2 = ecs.GetComponent<BoxColliderComponent>(e2);
                        
                        if (IsRestricted(b1.m_collisionTag, b2.m_collisionTag))
                            continue;
                   
                        TransformComponent& t2 = ecs.GetComponent<TransformComponent>(e2);
                        CheckCollision(t1, t2, b1, b2, e1, e2, collisions);
                    }
                    else if (ecs.HasComponent<SphereColliderComponent>(e2)) {
                        SphereColliderComponent& s2 = ecs.GetComponent<SphereColliderComponent>(e2);

                        if (IsRestricted(b1.m_collisionTag, s2.m_collisionTag))
                            continue;

                        TransformComponent& t2 = ecs.GetComponent<TransformComponent>(e2);
                        CheckCollision(t1, t2, b1, s2, e1, e2, collisions);
                    }
                }
            }
        );

        ecs.ForEach<SphereColliderComponent, TransformComponent, MeshRendererComponent>(
            [&](u32 e1, SphereColliderComponent& s1, TransformComponent& t1, MeshRendererComponent& m) {
                Vector<u32> nearby;
                m_spatialHash.GetNearby(e1, s1.m_box, nearby);

                for (u32 e2 : nearby)
                {
                    if (e1 >= e2) continue;

                    if (ecs.HasComponent<BoxColliderComponent>(e2)) {
                        BoxColliderComponent& b2 = ecs.GetComponent<BoxColliderComponent>(e2);

                        if (IsRestricted(s1.m_collisionTag, b2.m_collisionTag))
                            continue;

                        TransformComponent& t2 = ecs.GetComponent<TransformComponent>(e2);
                        CheckCollision(t1, t2, s1, b2, e1, e2, collisions);
                    }
                    else if (ecs.HasComponent<SphereColliderComponent>(e2)) {
                        SphereColliderComponent& s2 = ecs.GetComponent<SphereColliderComponent>(e2);

                        if (IsRestricted(s1.m_collisionTag, s2.m_collisionTag))
                            continue;

                        TransformComponent& t2 = ecs.GetComponent<TransformComponent>(e2);
                        CheckCollision(t1, t2, s1, s2, e1, e2, collisions);
                    }
                }
            }
        );

        ProcessCollisionsIntersection(ecs, collisions);
        ProcessCollisionsOnCollide(collisions);
	}

    void ColliderSystem::AddRestriction(int tag1, int tag2)
    {
        if (tag1 == tag2)
        {
            m_ignoredCollisions.push_back({ tag1, tag2 });
            return;
        }

        m_ignoredCollisions.push_back({ tag1, tag2 });
        m_ignoredCollisions.push_back({ tag2, tag1 });
    }

    CollisionInfo ColliderSystem::CalculateCollisionInfo(const DirectX::BoundingOrientedBox& box1, const DirectX::BoundingOrientedBox& box2)
    {
        CollisionInfo info;

        XMVECTOR center1 = XMLoadFloat3(&box1.Center);
        XMVECTOR center2 = XMLoadFloat3(&box2.Center);
        XMVECTOR delta = XMVectorSubtract(center2, center1);

        XMMATRIX rot1 = XMMatrixRotationQuaternion(XMLoadFloat4(&box1.Orientation));
        XMMATRIX rot2 = XMMatrixRotationQuaternion(XMLoadFloat4(&box2.Orientation));

        XMVECTOR ext1 = XMLoadFloat3(&box1.Extents);
        XMVECTOR ext2 = XMLoadFloat3(&box2.Extents);

        float minPenetration = FLT_MAX;
        XMVECTOR bestAxis = XMVectorZero();

        Vector<XMVECTOR> axes;

        axes.push_back(rot1.r[0]);
        axes.push_back(rot1.r[1]);
        axes.push_back(rot1.r[2]);

        axes.push_back(rot2.r[0]);
        axes.push_back(rot2.r[1]);
        axes.push_back(rot2.r[2]);

        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                XMVECTOR cross = XMVector3Cross(rot1.r[i], rot2.r[j]);
                if (XMVectorGetX(XMVector3LengthSq(cross)) > 1e-6f)
                {
                    axes.push_back(XMVector3Normalize(cross));
                }
            }
        }

        for (const auto& axis : axes)
        {
            XMVECTOR proj1Vec = XMVectorMultiply(ext1, XMVectorAbs(XMVector3TransformNormal(axis, XMMatrixInverse(nullptr, rot1))));
            float proj1 = XMVectorGetX(XMVector3Dot(proj1Vec, XMVectorSplatOne()));

            XMVECTOR proj2Vec = XMVectorMultiply(ext2, XMVectorAbs(XMVector3TransformNormal(axis, XMMatrixInverse(nullptr, rot2))));
            float proj2 = XMVectorGetX(XMVector3Dot(proj2Vec, XMVectorSplatOne()));

            float dist = std::abs(XMVectorGetX(XMVector3Dot(delta, axis)));

            float penetration = (proj1 + proj2) - dist;

            if (penetration < -1e-4f) 
            {
                info.penetrationDepth = 0;
                return info;
            }

            if (penetration < minPenetration)
            {
                minPenetration = penetration;
                bestAxis = axis;

                float sign = XMVectorGetX(XMVector3Dot(delta, axis));
                if (sign < 0)
                {
                    bestAxis = XMVectorNegate(bestAxis);
                }
            }
        }

        info.penetrationDepth = minPenetration;
        XMStoreFloat3(&info.m_normal, XMVector3Normalize(bestAxis));

        XMVECTOR contact = XMVectorLerp(center1, center2, 0.5f);
        XMStoreFloat3(&info.m_contactPoint, contact);

        return info;
    }

    CollisionInfo ColliderSystem::CalculateCollisionInfo(const DirectX::BoundingOrientedBox& box, const DirectX::BoundingSphere& sphere)
    {
        CollisionInfo info;

        XMVECTOR sphereCenter = XMLoadFloat3(&sphere.Center);
        XMVECTOR boxCenter = XMLoadFloat3(&box.Center);

        XMVECTOR delta = XMVectorSubtract(sphereCenter, boxCenter);

        XMMATRIX boxRot = XMMatrixRotationQuaternion(XMLoadFloat4(&box.Orientation));

        XMVECTOR localDelta = XMVector3TransformNormal(delta, XMMatrixInverse(nullptr, boxRot));

        XMVECTOR boxExtents = XMLoadFloat3(&box.Extents);

        XMVECTOR localClosest = XMVectorClamp(localDelta,
            XMVectorNegate(boxExtents),
            boxExtents);

        XMVECTOR worldClosest = XMVectorAdd(boxCenter,
            XMVector3TransformNormal(localClosest, boxRot));

        XMVECTOR sphereToClosest = XMVectorSubtract(worldClosest, sphereCenter);
        float distance = XMVectorGetX(XMVector3Length(sphereToClosest));

        if (distance > 0) 
        {
            XMVECTOR normal = XMVector3Normalize(XMVectorNegate(sphereToClosest));
            XMStoreFloat3(&info.m_normal, normal);
        }
        else
        {
            XMStoreFloat3(&info.m_normal, XMVectorSet(0, 1, 0, 0));
        }

        info.penetrationDepth = sphere.Radius - distance;

        if (distance > 0) {
            XMVECTOR contact = XMVectorAdd(worldClosest,
                XMVectorMultiply(
                    XMLoadFloat3(&info.m_normal),
                    XMVectorReplicate(distance * 0.5f)
                )
            );
            XMStoreFloat3(&info.m_contactPoint, contact);
        }
        else {
            info.m_contactPoint = sphere.Center;
        }

        return info;
    }

    CollisionInfo ColliderSystem::CalculateCollisionInfo(const BoundingSphere& sphere, const BoundingOrientedBox& box)
    {
        CollisionInfo info = CalculateCollisionInfo(box, sphere);
        XMVECTOR vNormal = XMLoadFloat3(&info.m_normal);
        vNormal = XMVectorScale(vNormal, -1.f);
        XMStoreFloat3(&info.m_normal, vNormal);
        return info;
    }

    CollisionInfo ColliderSystem::CalculateCollisionInfo(const DirectX::BoundingSphere& sphere1, const DirectX::BoundingSphere& sphere2)
    {
        CollisionInfo info;

        XMVECTOR center1 = XMLoadFloat3(&sphere1.Center);
        XMVECTOR center2 = XMLoadFloat3(&sphere2.Center);
        XMVECTOR delta = XMVectorSubtract(center2, center1);

        float distance = XMVectorGetX(XMVector3Length(delta));
        float radiusSum = sphere1.Radius + sphere2.Radius;

        if (distance > 0) 
            XMStoreFloat3(&info.m_normal, XMVector3Normalize(delta));
        else 
            XMStoreFloat3(&info.m_normal, XMVectorSet(1, 0, 0, 0));
        

        info.penetrationDepth = radiusSum - distance;

        XMVECTOR contact = XMVectorAdd(center1,
            XMVectorMultiply(
                XMLoadFloat3(&info.m_normal),
                XMVectorReplicate(sphere1.Radius - info.penetrationDepth * 0.5f)
            )
        );
        XMStoreFloat3(&info.m_contactPoint, contact);

        return info;
    }

	void ColliderSystem::ProcessCollisionsIntersection(Ecs& ecs, Vector<CollisionInfo>& collisions)
	{
        for (size_t i = 0; i < collisions.size(); i++)
        {
            CollisionInfo& collision = collisions[i];
            if (collision.m_noIntersectionPush1 || collision.m_noIntersectionPush2)
                continue;

            u32 entity1 = collision.m_entity1;
            u32 entity2 = collision.m_entity2;

            TransformComponent* transform1 = collision.m_transform1;
            TransformComponent* transform2 = collision.m_transform2;

            XMFLOAT3 currentPosition1 = transform1->GetWorldPosition();
            XMFLOAT3 currentPosition2 = transform2->GetWorldPosition();

            bool hasPhysics1 = ecs.HasComponent<PhysicComponent>(entity1);
            bool hasPhysics2 = ecs.HasComponent<PhysicComponent>(entity2);

            XMVECTOR normal = XMLoadFloat3(&collision.m_normal);
            float depth = collision.penetrationDepth;

            XMFLOAT3 newPos1;
            XMFLOAT3 newPos2;

            if (hasPhysics1 && hasPhysics2)
            {
                PhysicComponent& physic1 = ecs.GetComponent<PhysicComponent>(entity1);
                PhysicComponent& physic2 = ecs.GetComponent<PhysicComponent>(entity2);

                XMVECTOR vel1 = XMLoadFloat3(&physic1.m_velocity);
                XMVECTOR vel2 = XMLoadFloat3(&physic2.m_velocity);

                XMVECTOR pos1 = XMLoadFloat3(&currentPosition1);
                XMVECTOR pos2 = XMLoadFloat3(&currentPosition2);

                if (!physic1.m_isKinematic && !physic2.m_isKinematic)
                {
                    float totalMass = physic1.m_mass + physic2.m_mass;
                    float ratio1 = physic2.m_mass / totalMass;
                    float ratio2 = physic1.m_mass / totalMass;

                    XMVECTOR sep1 = XMVectorMultiply(normal, XMVectorReplicate(depth * ratio1));
                    XMVECTOR sep2 = XMVectorMultiply(normal, XMVectorReplicate(depth * ratio2));

                    XMStoreFloat3(&newPos1, XMVectorSubtract(pos1, sep1));
                    XMStoreFloat3(&newPos2, XMVectorAdd(pos2, sep2));

                    transform1->SetWorldPosition(newPos1);
                    transform2->SetWorldPosition(newPos2);

                    XMVECTOR relVel = XMVectorSubtract(vel1, vel2);
                    float velAlongNormal = XMVectorGetX(XMVector3Dot(relVel, normal));

                    if (velAlongNormal > 0)
                    {
                        float restitution = (physic1.m_bounciness * physic2.m_mass + physic2.m_bounciness * physic1.m_mass) / totalMass;

                        float impulse = -(1 + restitution) * velAlongNormal / totalMass;

                        XMVECTOR impulse1 = XMVectorMultiply(normal, XMVectorReplicate(impulse * ratio1));
                        XMVECTOR impulse2 = XMVectorMultiply(normal, XMVectorReplicate(impulse * ratio2));

                        vel1 = XMVectorAdd(vel1, impulse1);
                        vel2 = XMVectorSubtract(vel2, impulse2);

                        XMStoreFloat3(&physic1.m_velocity, vel1);
                        XMStoreFloat3(&physic2.m_velocity, vel2);
                    }
                }
                else if (!physic1.m_isKinematic && physic2.m_isKinematic)
                {
                    pos1 = XMVectorSubtract(pos1, XMVectorMultiply(normal, XMVectorReplicate(depth)));
                    XMStoreFloat3(&newPos1, pos1);
                    transform1->SetWorldPosition(newPos1);

                    float velAlongNormal = XMVectorGetX(XMVector3Dot(vel1, normal));

                    if (velAlongNormal > 0)
                    {
                        vel1 = XMVectorSubtract(vel1,
                            XMVectorMultiply(normal,
                                XMVectorReplicate((1 + physic1.m_bounciness) * velAlongNormal)
                            )
                        );
                        XMStoreFloat3(&physic1.m_velocity, vel1);
                    }
                }
                else if(physic1.m_isKinematic && !physic2.m_isKinematic)
                {
                    pos2 = XMVectorAdd(pos2, XMVectorMultiply(normal, XMVectorReplicate(depth)));
                    XMStoreFloat3(&newPos2, pos2);
                    transform2->SetWorldPosition(newPos2);

                    float velAlongNormal = XMVectorGetX(XMVector3Dot(vel2, normal));

                    if (velAlongNormal > 0)
                    {
                        vel2 = XMVectorAdd(vel2,
                            XMVectorMultiply(normal,
                                XMVectorReplicate((1 + physic2.m_bounciness) * velAlongNormal)
                            )
                        );
                        XMStoreFloat3(&physic2.m_velocity, vel2);
                    }
                }
            }
        }
	}

	void ColliderSystem::ProcessCollisionsOnCollide(Vector<CollisionInfo>& collisions)
	{
        for (size_t i = 0; i < collisions.size(); i++)
        {
            CollisionInfo& collision = collisions[i];

            u32 entity1 = collision.m_entity1;
            u32 entity2 = collision.m_entity2;

            if (collision.OnCollision1)
                collision.OnCollision1(entity1, entity2, collision);

            if (collision.OnCollision2)
                collision.OnCollision2(entity2, entity1, collision);
        }
	}
    bool ColliderSystem::IsRestricted(int tag1, int tag2) const
    {
        if (tag1 == -1 || tag2 == -1)
            return false;

        for (auto& pair : m_ignoredCollisions)
        {
            if (pair.first == tag1 && pair.second == tag2)
                return true;
        }

        return false;
    }
}
