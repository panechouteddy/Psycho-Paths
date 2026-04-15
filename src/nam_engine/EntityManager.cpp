#include "pch.h"
#include "EntityManager.h"

namespace nam
{
    Entity EntityManager::CreateEntity()
    {
        if (m_deadEntities.empty())
            return Entity(++m_lastEntityId, 1);

        Entity& entity = m_deadEntities.back();
        m_deadEntities.pop_back();

        entity.m_generation++;
        return entity;
    }

    void EntityManager::ReleaseEntity(Entity& entity)
    {
        m_deadEntities.push_back(entity);
    }
}