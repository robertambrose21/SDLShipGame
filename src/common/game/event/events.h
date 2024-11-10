#pragma once

#include <vector>
#include "core/glmimport.h"
#include "core/util/idgenerator.h"
#include "game/effects/effecttypes.h"
#include "game/stats/stats.h"

class Entity;
class Item;
class AreaOfEffect;
class Weapon;
class Projectile;
class Grid;

struct TurnEventData {
    int turnNumber;
    int currentParticipant;
};

struct ItemEventData {
    enum Type {
        SPAWN,
        REMOVED
    };

    Entity* owner;
    std::vector<Item*> items;
    Type type;
};

struct AreaOfEffectEventData {
    AreaOfEffect* aoe;
    Entity* target;
    int damage;
};

struct ProjectileEventData {
    Projectile* projectile;
    Entity* target;
    glm::ivec2 hitPosition;
    int damage;
};

struct MeleeWeaponEventData {
    Entity* owner;
    Entity* target;
    Weapon* weapon;
    int damage;
};

struct MoveActionEventData {
    int turnNumber;
    Entity* entity;
    glm::ivec2 position;
    int shortStopSteps;
};

struct AttackActionEventData {
    int turnNumber;
    Entity* owner;
    glm::ivec2 target;
    Weapon* weapon;
};

struct TakeItemActionEventData {
    int turnNumber;
    Entity* entity;
    std::vector<Item*> items;
};

enum DamageType {
    AOE,
    PROJECTILE,
    MELEE
};

struct ApplyDamageEventData {
    int participantId;
    Entity* target;
    DamageType source;
    int damage;
};

struct EngagementEventData {
    enum Type {
        ENGAGED,
        DISENGAGED
    };

    int participantIdA;
    int participantIdB;
    Type type;
};

struct TileEventData {
    int x, y;
    int id;
    bool isWalkable;
    bool isFrozen;
};

struct EquipItemActionEventData {
    int turnNumber;
    Entity* entity;
    Item* item;
    int slot;
    bool isUnequip;
};

struct EquipWeaponActionEventData {
    int turnNumber;
    Entity* entity;
    Item* item;
    UUID weaponId;
};

struct EntityEffectEvent {
    EffectType type;
    Entity* target;
    EffectStats stats;
};

struct GridEffectEvent {
    EffectType type;
    int x, y;
    int duration;
};

struct GridDirtyEventData {
    bool isGridDirty;
};

struct TilesRevealedEventData {
    struct RevealedTile {
        int id;
        int x, y;
    };

    int participantId;
    std::vector<RevealedTile> tiles;
};

struct EntitySetPositionEventData {
    Entity* entity;
    glm::ivec2 position;
};

struct EntityVisibilityToParticipantData {
    Entity* entity;
    int participantId;
    bool isVisible;
};
