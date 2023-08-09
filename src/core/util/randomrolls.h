#pragma once

#include <cstdint>
#include <random>

inline uint32_t randomRange(uint32_t lower, uint32_t upper) {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(lower, upper);

    return dist(rng);
}

inline uint32_t randomDN(uint32_t n) {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> distn(1, n);

    return distn(rng);
}

inline uint32_t randomD6(void) {
    return randomDN(6);
}