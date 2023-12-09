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
        SPAWN
    };

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

