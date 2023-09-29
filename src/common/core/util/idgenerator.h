#pragma once

#include <cstdint>

inline uint32_t getNewId(void) {
    static uint32_t id = 0;
    return id++;
}