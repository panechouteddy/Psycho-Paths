#pragma once
#include "Ecs.h"
#include "ComponentId.h"
#include "SystemId.h"
#include "ViewId.h"
#include "View.h"

namespace nam
{
    template<typename System, typename... Args>
    inline System* Ecs::AddSystem(Args&&... args)
    {
        u32 systemId = SystemId::Get<System>();
        assert(!m_systems[systemId] && "adding existing system");

        System* s = new System(std::forward<Args>(args)...);

        m_systems[systemId] = s;
        m_systems[systemId]->Init();

        return s;
    }

    template<typename System>
    inline void Ecs::SetSystemEnabled(const bool enabled)
    {
        u32 systemId = SystemId::Get<System>();
        assert(m_systems[systemId] && "system not existant");
        m_systems[systemId]->SetEnabled(enabled);
    }

    template<typename Component>
    inline void Ecs::AddComponent(Entity& entity, const Component& data)
    {
        assert(IsEntityAlive(entity) && "operation on dead entity");
        GetSet<Component>().Add(entity.m_id, data);
        m_componentToRefresh.push_back(ComponentId::Get<Component>());
    }

    template<typename Component>
    inline Component& Ecs::GetComponent(Entity& entity)
    {
        assert(IsEntityAlive(entity) && "operation on dead entity");
        assert(HasComponent<Component>(entity.m_id) && "entity doesn't have the component");
        return GetComponent<Component>(entity.m_id);
    }
    
    template<typename Component>
    inline Component& Ecs::GetComponent(u32 entityId)
    {
        return GetSet<Component>().Get(entityId);
    }

    template<typename Component>
    inline bool Ecs::HasComponent(Entity& entity)
    {
        assert(IsEntityAlive(entity) && "operation on dead entity");
        return HasComponent<Component>(entity.m_id);
    }


    template<typename Component>
    inline bool Ecs::HasComponent(u32 entityId)
    {
        return GetSet<Component>().Has(entityId);
    }

    template<typename... Components, typename Func>
    inline void Ecs::ForEach(Func&& func) {
        u32 viewId = ViewId::Get<Components...>();
        View<Components...>* view = nullptr;
        if (viewId >= m_views.size())
        {
            view = new View<Components...>(*this);
            m_views.push_back(view);
        }
        else
        {
            view = static_cast<View<Components...>*>(m_views[viewId]);
        }

        view->Refresh(*this);
        view->ForEach(*this, func);
    }

    template<typename ...Components, typename Func>
    inline void Ecs::DoubleForEach(Func&& func)
    {
        u32 viewId = ViewId::Get<Components...>();
        View<Components...>* view = nullptr;
        if (viewId >= m_views.size())
        {
            view = new View<Components...>(*this);
            m_views.push_back(view);
        }
        else
        {
            view = static_cast<View<Components...>*>(m_views[viewId]);
        }

        view->Refresh(*this);
        view->DoubleForEach(*this, func);
    }

    template<typename ...Components>
    inline ISparseSet* Ecs::GetSmallestComponentSet()
    {
        if constexpr (sizeof...(Components) == 0) {
            return nullptr; //should not happen
        }

        ISparseSet* sets[] = { &GetSet<Components>()... };

        ISparseSet* p_smallest = sets[0];
        size smallestSize = p_smallest->Size();

        for (size i = 1; i < sizeof...(Components); ++i) {
            size currentSize = sets[i]->Size();
            if (currentSize < smallestSize) {
                p_smallest = sets[i];
                smallestSize = currentSize;
            }
        }

        return p_smallest;
    }

    template<typename Component>
    inline SparseSet<Component>& Ecs::GetSet()
    {
        u32 componentId = ComponentId::Get<Component>();

        if (!m_components[componentId]) {
            m_components[componentId] = new SparseSet<Component>(EcsConfig::MAX_ENTITIES);
        }
        return *static_cast<SparseSet<Component>*>(m_components[componentId]);
    }
}