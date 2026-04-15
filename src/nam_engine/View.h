#pragma once
#include "IView.h"
#include "ContainerComposite.h"

namespace nam
{
	class Ecs;

	template<typename... Components>
	class View : public IView
	{
	private:
		SparseSet<u8> m_componentsId; // can't use bool because can't get bool&
		Vector<u32> m_entities;
		Tuple<SparseSet<Components>*...> m_cachedSets;
		Tuple<Vector<Components*>...> m_soaData;
		bool m_needRefresh = true;

	public:
		View(Ecs& ecs);
		~View() override = default;

		void ActiveNeedRefresh() override;

		template<typename Func>
		void ForEach(Ecs& ecs, Func&& func);

		template<typename Func>
		void DoubleForEach(Ecs& ecs, Func&& func);

		void Refresh(Ecs& ecs) override;
		bool HasComponent(const u32 componentId) const override;
	};
}

#include "View.inl"