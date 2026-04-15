#pragma once

namespace nam
{
    struct Entity;

    class EntityManager
    {

    private:
        Vector<Entity> m_deadEntities;
        u32 m_lastEntityId = 0;

    public:
        Entity CreateEntity();
        void ReleaseEntity(Entity& entity);
    };
}
