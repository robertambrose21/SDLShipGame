#pragma once

#include <string>

typedef struct _effect {
    std::string name;
    int duration;

    _effect(const std::string& name, int duration) :
        name(name),
        duration(duration)
    { }
} Effect;
