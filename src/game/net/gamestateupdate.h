#pragma once

#include <cstdint>
#include <cstring>

const int MaxEntities = 64;

struct EntityStateUpdate {
    uint32_t id;
    int currentHP;
    int x, y;

    EntityStateUpdate() {
        memset(this, 0, sizeof(EntityStateUpdate));
    }
};

struct GameStateUpdate {
    int numEntities;
    EntityStateUpdate entities[MaxEntities];

    GameStateUpdate() {
        memset(this, 0, sizeof(GameStateUpdate));
    }
};