#pragma once

#include <ctime>

template<typename T>
struct Event {
    T data;
    std::time_t timestamp;
};