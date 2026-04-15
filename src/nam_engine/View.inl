#include "View.h"
#pragma once

namespace nam
{
	template<typename ...Components>
	inline View<Components...>::View(Ecs& ecs) : m_componentsId(static_cast<size>(EcsConfig::MAX_COMPONENTS))
	{
		(m_componentsId.Add(ComponentId::Get<Components>(), 1), ...);
		m_cachedSets = std::make_tuple(&ecs.GetSet<Components>()...);

		m_soaData = std::make_tuple(Vector<Components*>()...);
	};

	template<typename ...Components>
	inline void View<Components...>::ActiveNeedRefresh()
	{
		m_needRefresh = true;
	}

	template<typename ...Components>
	template<typename Func>
	inline void View<Components...>::ForEach(Ecs& ecs, Func&& func) {
		const size count = m_entities.size();
		std::apply([&](Vector<Components*>... vectors)
			{
				for (size i = 0; i < count; ++i)
				{
					func(
						m_entities[i],
						*vectors[i]...
					);
				}
			}, m_soaData);
	}

	template<typename ...Components>
	template<typename Func>
	inline void View<Components...>::DoubleForEach(Ecs& ecs, Func&& func)
	{
		const size count = m_entities.size();
		std::apply([&](Vector<Components*>... vectors)
			{
				for (size i = 0; i < count; ++i)
				{
					for (size j = i + 1; j < count; ++j)
					{
						func(
							m_entities[i],
							*vectors[i]...,
							m_entities[j],
							*vectors[j]...
						);
					}
				}
			}, m_soaData);
	}

	template<typename ...Components>
	inline void View<Components...>::Refresh(Ecs& ecs)
	{
		if (!m_needRefresh)
			return;

		m_entities.clear();
		std::apply([](auto&... vec) {
			(vec.clear(), ...);
		}, m_soaData);

		ISparseSet* p_smallest = ecs.GetSmallestComponentSet<Components...>();
		if (!p_smallest) return;

		Vector<u32>& entities = m_entities;
		Tuple<SparseSet<Components>*...>& cachedSets = m_cachedSets;

		m_entities.reserve(p_smallest->Size());
		std::apply([&](auto&... vec) {
			(vec.reserve(p_smallest->Size()), ...);
		}, m_soaData);

		Vector<u32> smallestEntities = p_smallest->GetEntities();
		
		for (size_t i = 0; i < smallestEntities.size(); i++)
		{
			u32 entityId = smallestEntities[i];

			if (!ecs.IsEntityActive(entityId))
				continue;

			if (!(std::get<SparseSet<Components>*>(cachedSets)->Has(entityId) && ...))
			{
				continue;
			}

			entities.push_back(entityId);

			std::apply([&](auto&... vec) {
				(vec.push_back(&std::get<SparseSet<Components>*>(cachedSets)->Get(entityId)), ...);
				}, m_soaData);
		}

		m_needRefresh = false;
	}

	template<typename ...Components>
	inline bool View<Components...>::HasComponent(const u32 componentId) const
	{
		return m_componentsId.Has(componentId);
	}
}