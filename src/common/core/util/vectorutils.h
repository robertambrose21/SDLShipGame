#pragma once

#include <vector>

template<typename T>
inline bool contains(const std::vector<T>& vec, const T& item) {
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