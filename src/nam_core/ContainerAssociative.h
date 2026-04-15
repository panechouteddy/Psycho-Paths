#pragma once

namespace nam
{
    template<typename K, typename Compare = std::less<K>>
    using Set = std::set<K, Compare>;

    template<typename K, typename Compare = std::less<K>>
    using MSet = std::multiset<K, Compare>;

    template<typename K, typename V, typename Compare = std::less<K>>
    using Map = std::map<K, V, Compare>;

    template<typename K, typename V, typename Compare = std::less<K>>
    using MMap = std::multimap<K, V, Compare>;
}