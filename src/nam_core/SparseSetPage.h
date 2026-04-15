#pragma once
#include <BaseTypes.h>
#include <bitset>

namespace nam
{
    template<typename DenseType>
    struct SparseSetPage {
        static constexpr u32 PAGE_SIZE = 4096;

        DenseType m_data[PAGE_SIZE] = {};
        u32 m_entities[PAGE_SIZE] = {};

        u32 m_count = 0;
        Vector<u16> m_freeLocal;
        std::bitset<PAGE_SIZE> m_maskFreeLocal;
    };
}
