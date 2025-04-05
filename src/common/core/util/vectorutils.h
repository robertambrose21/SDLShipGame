#pragma once

#include <vector>
#include <ranges>
#include <array>
#include <map>
#include <set>

// TODO: namespace all these
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

template<typename T>
inline void eraseByValue(const std::vector<T>& vec, const T& value) {
    std::erase_if(vec, [&](const auto& item) {
        return item == value;
    });
}

template<typename K, typename V>
inline std::vector<K> getKeys(const std::map<K, V>& map) {
    auto keyRange = std::views::keys(map);
    return std::vector<K> { keyRange.begin(), keyRange.end() };
}

// TODO: Move to a set utils file
template<typename T>
inline bool containsAll(const std::set<T>& s, const std::set<T>& items) {
    for(auto& item : items) {
        if(s.count(item) == 0) {
            return false;
        }
    }

    return true;
}

template<typename T>
inline void insertAll(std::vector<T>& vec, const std::vector<T>& other) {
    vec.insert(std::end(vec), std::begin(other), std::end(other));
}
