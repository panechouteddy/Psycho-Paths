#pragma once


namespace nam
{
    template<typename K, typename Hash = std::hash<K>, typename KeyEq = std::equal_to<K>>
    using UnSet = std::unordered_set<K, Hash, KeyEq>;

    template<typename K, typename Hash = std::hash<K>, typename KeyEq = std::equal_to<K>>
    using UnMSet = std::unordered_multiset<K, Hash, KeyEq>;

    template<typename K, typename V, typename Hash = std::hash<K>, typename KeyEq = std::equal_to<K>>
    using UnMap = std::unordered_map<K, V, Hash, KeyEq>;

    template<typename K, typename V, typename Hash = std::hash<K>, typename KeyEq = std::equal_to<K>>
    using UnMMap = std::unordered_multimap<K, V, Hash, KeyEq>;
}
