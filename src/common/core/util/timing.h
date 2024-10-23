#pragma once

#include <chrono>

inline int64_t getCurrentTimeInMilliseconds(void) {
    auto currentTime = std::chrono::system_clock::now();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime.time_since_epoch());
    return milliseconds.count();
}

inline int64_t getCurrentTimeInNanoseconds(void) {
    auto currentTime = std::chrono::system_clock::now();
    auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime.time_since_epoch());
    return nanoseconds.count();
}

inline int64_t getCurrentTimeInMicroseconds(void) {
    auto currentTime = std::chrono::system_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(currentTime.time_since_epoch());
    return microseconds.count();
}