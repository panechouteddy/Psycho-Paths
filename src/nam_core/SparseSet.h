#pragma once
#include "ISparseSet.h"
#include "SparseSetPage.h"
#include "PageIndex.h"

namespace nam
{
	template<typename DenseType>
	class SparseSet : public ISparseSet
	{
	private:
		Vector<u32> m_sparse;
		Vector<SparseSetPage<DenseType>> m_pages;
		Vector<PageIndex> m_entityToPageIndex;

		static constexpr u32 NO_ENTITY = (std::numeric_limits<u32>::max)();
		static constexpr u32 INVALID_PAGE = (std::numeric_limits<u32>::max)();

	public:
		SparseSet(size sparseSize);
		~SparseSet();

		bool Add(const u32 id, const DenseType& denseType);
		bool Remove(const u32 id) override;
		bool Has(const u32 id) const override;
		const DenseType& Get(const u32 id) const;
		DenseType& Get(const u32 id);

		Vector<u32> GetEntities() const override;

		const size Size() const override;
	};
}
#include "SparseSet.inl"
