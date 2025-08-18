// TODO: It's odd having this class and the other messages.h - consider consolodating or coming up with a different strategy
#pragma once

#include <cstdint>
#include <cstring>
#include "game/actors/actor.h"

const int MaxActors = 8;
const int MaxActorNameLength = 64;
const int MaxWeapons = 8;
const int MaxWeaponNameLength = 64;

class Actor;
class Enemy;
class Player;
class Weapon;

struct WeaponStateUpdate {
    uint8_t idBytes[16];
    char name[MaxWeaponNameLength];
    char weaponClass[MaxWeaponNameLength];
    char projectile[MaxWeaponNameLength];
    int damage;
    int range;
    int uses;
    int usesLeft;
    uint32_t itemId;
    bool hasItem;

    WeaponStateUpdate() {
        memset(this, 0, sizeof(WeaponStateUpdate));
    }

    static WeaponStateUpdate serialize(Weapon* weapon);
    static Weapon* deserialize(const WeaponStateUpdate& update, Weapon* existing);
};

// TODO: Send weapon state updates separately if actor has more than the max number of weapons
// TODO: Serialise ActorStats
struct ActorStateUpdate {
    uint32_t id;
    char name[MaxActorNameLength];
    int totalHP;
    int currentHP;
    int x, y;
    int participantId;
    bool isEngaged;
    uint8_t currentWeaponIdBytes[16];
    int numWeapons;
    WeaponStateUpdate weaponUpdates[MaxWeapons];

    ActorStateUpdate() {
        memset(this, 0, sizeof(ActorStateUpdate));
    }

    static ActorStateUpdate serialize(Actor* actor);
    static void deserialize(const ActorStateUpdate& update, Actor* existing);
};

struct GameStateUpdate {
    int numActors;
    ActorStateUpdate actors[MaxActors];
    int currentParticipantId;
    uint8_t numExpectedChunks;
    uint32_t chunkId;

    GameStateUpdate() {
        memset(this, 0, sizeof(GameStateUpdate));
    }

    static GameStateUpdate serialize(
        int currentParticipantId, 
        const std::vector<Actor*>& actors,
        uint32_t chunkId,
        uint8_t numExpectedChunks = 1
    ) {
        GameStateUpdate update;
        update.numActors = actors.size();

        int index = 0;
        for(auto actor : actors) {
            update.actors[index++] = ActorStateUpdate::serialize(actor);
        }

        update.currentParticipantId = currentParticipantId;
        update.numExpectedChunks = numExpectedChunks;

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

    ItemUpdate() {
        memset(this, 0, sizeof(ItemUpdate));
    }
};

struct EffectStatsUpdate {
    uint8_t effectType;
    uint8_t duration;
    uint8_t numDamageTicks;
    uint32_t damageTicks[64];

    EffectStatsUpdate() {
        memset(this, 0, sizeof(EffectStatsUpdate));
    }
};
