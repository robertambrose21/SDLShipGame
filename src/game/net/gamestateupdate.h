#pragma once

#include <cstdint>

const int MaxEntities = 64;

struct EntityStateUpdate {
    uint32_t id;
    int currentHP;
    int x, y;
};

struct GameStateUpdate {
    int numEntities;
    EntityStateUpdate entities[MaxEntities];
};