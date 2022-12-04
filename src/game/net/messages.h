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

    static WeaponStateUpdate serialize(std::shared_ptr<Weapon> weapon);
    static std::shared_ptr<Weapon> deserialize(WeaponStateUpdate update, std::shared_ptr<Weapon> existing);
};

// TODO: Send weapon state updates separately if entity has more than the max number of weapons
struct EntityStateUpdate {
    uint32_t id;
    char name[MaxEntityNameLength];
    int totalHP;
    int currentHP;
    int x, y;
    int movesPerTurn;
    int movesLeft;
    int participantId;
    uint32_t currentWeaponId;
    int numWeapons;
    WeaponStateUpdate weaponUpdates[MaxWeapons];

    EntityStateUpdate() {
        memset(this, 0, sizeof(EntityStateUpdate));
    }

    static EntityStateUpdate serialize(std::shared_ptr<Entity> entity);
    static std::shared_ptr<Entity> deserialize(EntityStateUpdate update, std::shared_ptr<Entity> existing);
};

struct GameStateUpdate {
    int numEntities;
    EntityStateUpdate entities[MaxEntities];
    int currentParticipant;

    GameStateUpdate() {
        memset(this, 0, sizeof(GameStateUpdate));
    }

    static GameStateUpdate serialize(
        const int& currentParticipant, 
        std::map<uint32_t, std::shared_ptr<Entity>> entities
    ) {
        GameStateUpdate update;
        update.numEntities = entities.size();

        int index = 0;
        for(auto [_, entity] : entities) {
            update.entities[index++] = EntityStateUpdate::serialize(entity);
        }

        update.currentParticipant = currentParticipant;

        return update;
    }
};

const int MaxMapBlockSize = 512;

struct MapBlock {
    int width;
    int height;
    int totalSize;
    int sequence;
    int blockSize;

    int data[MaxMapBlockSize];

    MapBlock() {
        memset(this, 0, sizeof(MapBlock));
    }
};