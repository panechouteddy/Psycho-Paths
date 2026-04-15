#pragma once
#include "BaseTypes.h"
#include "ContainerSequential.h"
#include "EcsConfig.h"
#include "EntityManager.h"
#include "Entity.h"

#include "SparseSet.h"

namespace nam
{
    class ISystem;
    class ISparseSet;
    class IView;

    class Ecs
    {
    private:
        ISystem* m_systems[EcsConfig::MAX_SYSTEMS];

        ISparseSet* m_components[EcsConfig::MAX_COMPONENTS];

        EntityManager m_entityManager;
        Vector<Entity> m_entitiesToClean;
        SparseSet<u8> m_entitiesAlive;
        SparseSet<u8> m_entitiesActive;

        Vector<IView*> m_views;
        Vector<u32> m_componentToRefresh;

    public:
        Ecs();
        ~Ecs();

        void Update();
        void CleanUpdate();

        Entity CreateEntity();
        void DestroyEntity(Entity& entity);
        void SetEntityActive(Entity& entity, bool active);
        bool IsEntityAlive(Entity& entity) const;
        bool IsEntityActive(u32 entityId) const;

        template<typename System, typename... Args>
        System* AddSystem(Args&&... args);

        template<typename System>
        void SetSystemEnabled(const bool enabled);

        template<typename Component>
        void AddComponent(Entity& entity, const Component& data);

        template<typename Component>
        Component& GetComponent(Entity& entity);
        template<typename Component>
        Component& GetComponent(u32 entityId);

        template<typename Component>
        bool HasComponent(Entity& entity);
        template<typename Component>
        bool HasComponent(u32 entityId);
        

        template<typename... Components, typename Func>
        void ForEach(Func&& func);

        template<typename... Components, typename Func>
        void DoubleForEach(Func&& func);

    private:
        void UpdateSystems();
        void CleanupEntities();
        void ActiveRefreshView();

        template<typename... Components>
        ISparseSet* GetSmallestComponentSet();

        template<typename Component>
        SparseSet<Component>& GetSet();

        friend class IView;
        template<typename... Components> friend class View;
    };
}
#include "Ecs.inl"
