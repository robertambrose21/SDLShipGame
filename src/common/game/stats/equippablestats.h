#pragma once

#include <cstdint>

#include "game/effects/effecttypes.h"

// When adding new stats, ensure they have a default value set
typedef struct _equippableStats {
    uint32_t armour = 0;
    uint32_t hp = 0;
    uint8_t speed = 0;
    uint8_t power = 0;
    uint8_t wisdom = 0;
} EquippableStats;

typedef struct _equipmentStats : public EquippableStats {
} EquipmentStats;

typedef struct _gearStats : public EquipmentStats {
} GearStats;

typedef struct _damageStats {
    uint8_t numDice = 0;
    uint8_t diceSize = 0;
    uint32_t flatDamage = 0;
    uint8_t power = 0;
} DamageStats2;

typedef struct _aoeStats {
    DamageStats2 damage;
    float radius = 0;
    uint8_t duration = 0;
} AoEStats;

typedef struct _effectStats {
    EffectType type;
    uint8_t duration = 0;
    std::vector<uint32_t> damageTicks;
} EffectStats2;

typedef struct _projectileStats {
    float speed;
    std::vector<EffectStats2> effects;
    AoEStats aoe;
} ProjectileStats2;

typedef struct _weaponStats : public EquipmentStats {
    DamageStats2 damage;
    uint8_t uses = 0;
    uint8_t range = 0;
} WeaponStats2;

typedef struct _entityStats : public EquipmentStats {
    uint32_t totalHp = 0;
    uint8_t movesPerTurn = 0;
    uint8_t movesLeft = 0;
} EntityStats;

typedef struct _itemStats { 
    GearStats gear;
    WeaponStats2 weapon;
} ItemStats;
