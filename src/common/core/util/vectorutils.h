#pragma once

#include <vector>
#include <ranges>
#include <array>

template<typename T>
inline bool contains(const std::vector<T>& vec, const T& item) {
    return std::find(vec.begin(), vec.end(), item) != vec.end();
}

template<typename T, std::size_t S>
inline bool contains(const std::array<T, S>& vec, const T& item) {
    return std::find(vec.begin(), vec.end(), item) != vec.end();
}

template<typename T>
inline bool containsAny(const std::vector<T>& vec, const std::vector<T>& items) {
    for(auto& item : items) {
        if(contains(vec, item)) {
            return true;
        }
    }

    return false;
}

template<typename T>
inline bool containsAll(const std::vector<T>& vec, const std::vector<T>& items) {
    for(auto& item : items) {
        if(!contains(vec, item)) {
            return false;
        }
    }
    
    return true;
}

template<typename K, typename V>
inline std::vector<K> getKeys(const std::map<K, V>& map) {
    auto keyRange = std::views::keys(map);
    return std::vector<K> { keyRange.begin(), keyRange.end() };
}
