#pragma once

#include <chrono>

inline int64_t getCurrentTimeInMilliseconds(void) {
    auto currentTime = std::chrono::system_clock::now();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime.time_since_epoch());
    return milliseconds.count();
}