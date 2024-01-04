#pragma once

#include <cstdint>
#include <cstring>
#include "game/entities/entity.h"

const int MaxEntities = 8;
const int MaxEntityNameLength = 64;
const int MaxWeapons = 8;
const int MaxWeaponNameLength = 64;

class Entity;
class Enemy;
class Player;
class Weapon;

struct WeaponStateUpdate {
    uint32_t id;
    char name[MaxWeaponNameLength];
    char weaponClass[MaxWeaponNameLength];
    char projectile[MaxWeaponNameLength];
    int damage;
    int range;
    int uses;
    int usesLeft;

    WeaponStateUpdate() {
        memset(this, 0, sizeof(WeaponStateUpdate));
    }

    static WeaponStateUpdate serialize(Weapon* weapon);
    static Weapon* deserialize(const WeaponStateUpdate& update, Weapon* existing);
};

// TODO: Send weapon state updates separately if entity has more than the max number of weapons
struct EntityStateUpdate {
    uint32_t id;
    char name[MaxEntityNameLength];
    int totalHP;
    int currentHP;
    int x, y;
    int participantId;
    uint32_t currentWeaponId;
    int numWeapons;
    WeaponStateUpdate weaponUpdates[MaxWeapons];

    EntityStateUpdate() {
        memset(this, 0, sizeof(EntityStateUpdate));
    }

    static EntityStateUpdate serialize(Entity* entity);
    static void deserialize(const EntityStateUpdate& update, Entity* existing);
};

struct GameStateUpdate {
    int numEntities;
    EntityStateUpdate entities[MaxEntities];
    int currentParticipantId;

    GameStateUpdate() {
        memset(this, 0, sizeof(GameStateUpdate));
    }

    static GameStateUpdate serialize(
        int currentParticipantId, 
        const std::vector<Entity*>& entities
    ) {
        GameStateUpdate update;
        update.numEntities = entities.size();

        int index = 0;
        for(auto entity : entities) {
            update.entities[index++] = EntityStateUpdate::serialize(entity);
        }

        update.currentParticipantId = currentParticipantId;

        return update;
    }
};

const int MaxMapBlockSize = 512;

struct MapBlock {
    int width;
    int height;
    int totalSize;
    int sequence;
    int numSequences;
    int blockSize;

    int data[MaxMapBlockSize];

    MapBlock() {
        memset(this, 0, sizeof(MapBlock));
    }
};

struct DiceActionResult {
    int rollNumber;
    int actions[6] = { 0 };

    DiceActionResult() {
        memset(this, 0, sizeof(DiceActionResult));
    }
};

struct ItemUpdate {
    char name[64];
    uint32_t id;
};
