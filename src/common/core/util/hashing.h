#pragma once

#include <cstddef>

// Yoink from boost
inline void combineHashes(std::size_t& seed, std::size_t value) {
    seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template<typename... Types>
std::size_t constructHash(std::size_t seed, const Types&... types)  {
    (combineHashes(seed, std::hash<Types>{}(types)), ...);
    return seed;
}