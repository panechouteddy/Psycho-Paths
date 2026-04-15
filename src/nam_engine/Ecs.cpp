#include "pch.h"
#include "Ecs.h"

namespace nam
{
    Ecs::Ecs() : m_entitiesAlive(EcsConfig::MAX_ENTITIES), m_entitiesActive(EcsConfig::MAX_ENTITIES)
    {
        for (u32 i = 0; i < EcsConfig::MAX_COMPONENTS; ++i) {
            m_components[i] = nullptr;
        }

        for (u32 i = 0; i < EcsConfig::MAX_SYSTEMS; ++i) {
            m_systems[i] = nullptr;
        }
    }

    Ecs::~Ecs()
    {
        for (u32 i = 0; i < ComponentId::GetCurrentLast(); ++i) {
            delete m_components[i];
        }

        for (u32 i = 0; i < SystemId::GetCurrentLast(); ++i) {
            ISystem* p_system = m_systems[i];
            p_system->Destroy();
            delete p_system;
        }
    }

    void Ecs::Update()
    {
        UpdateSystems();
    }

    void Ecs::CleanUpdate()
    {
        CleanupEntities();
        ActiveRefreshView();
    }

    Entity Ecs::CreateEntity()
    {
        Entity entity = m_entityManager.CreateEntity();
        assert(entity.m_id < EcsConfig::MAX_ENTITIES && "max entity number reached");
        m_entitiesAlive.Add(entity.m_id, entity.m_generation);
        m_entitiesActive.Add(entity.m_id, 1);
        return entity;
    }

    void Ecs::DestroyEntity(Entity& entity)
    {
        if (IsEntityAlive(entity))
        {
            m_entitiesToClean.push_back(entity); // can have dupli but it's fine
        }
    }

    void Ecs::SetEntityActive(Entity& entity, bool active)
    {
        if (active)
            m_entitiesActive.Add(entity.m_id, 1);
        else
            m_entitiesActive.Remove(entity.m_id);

        for (u32 i = 0; i < ComponentId::GetCurrentLast(); ++i) {
            if (m_components[i]->Has(entity.m_id))
                m_componentToRefresh.push_back(i);
        }
    }

    bool Ecs::IsEntityAlive(Entity& entity) const
    {
        if (!m_entitiesAlive.Has(entity.m_id)) return false;
        return m_entitiesAlive.Get(entity.m_id) == entity.m_generation;
    }

    bool Ecs::IsEntityActive(u32 entityId) const
    {
        return m_entitiesActive.Has(entityId);
        //if (!m_entitiesActive.Has(entityId))
        //      return false;

    }

    void Ecs::UpdateSystems()
    {
        for (u32 i = 0; i < SystemId::GetCurrentLast(); ++i) {
            ISystem* p_system = m_systems[i];
            if (p_system->IsEnabled())
                p_system->Update(*this);
        }
    }

    void Ecs::CleanupEntities()
    {
        if (m_entitiesToClean.empty()) return;

        for (u32 i = 0; i < ComponentId::GetCurrentLast(); ++i) {
            if (m_components[i]) {
                for (Entity entity : m_entitiesToClean) {
                    m_components[i]->Remove(entity.m_id);
                    m_componentToRefresh.push_back(i);
                }
            }
        }

        for (Entity entity : m_entitiesToClean) {
            m_entitiesAlive.Remove(entity.m_id);
            m_entityManager.ReleaseEntity(entity);
            m_entitiesActive.Remove(entity.m_id);
        }

        m_entitiesToClean.clear();
    }

    void Ecs::ActiveRefreshView()
    {
        for (size i = 0; i < m_componentToRefresh.size(); i++)
        {
            u32 componentToRefresh = m_componentToRefresh[i];
            for (size j = 0; j < m_views.size(); j++)
            {
                IView* p_view = m_views[j];
                if (p_view->HasComponent(componentToRefresh))
                    p_view->ActiveNeedRefresh();
            }
        }

        m_componentToRefresh.clear();
    }
}