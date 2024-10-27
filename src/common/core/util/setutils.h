#pragma once

#include <unordered_set>

template<typename T, typename H>
inline bool contains(const std::unordered_set<T, H>& s, const T& item) {
    return s.find(item) != s.end();
}
