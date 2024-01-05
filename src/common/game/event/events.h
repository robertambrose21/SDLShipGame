#pragma once

#include <vector>
#include "core/glmimport.h"

class Entity;
class Item;
class AreaOfEffect;
class Weapon;
class Projectile;

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
};

struct ProjectileEventData {
    Projectile* projectile;
    Entity* target;
    glm::ivec2 hitPosition;
    int damage;
};

struct WeaponEventData {
    Entity* owner;
    Entity* target;
    Weapon* weapon;
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
