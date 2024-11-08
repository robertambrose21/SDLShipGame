#pragma once

#include <cstdint>
#include <vector>
#include <string>

#include "game/effects/effecttypes.h"

// When adding new stats, ensure they have a default value set
typedef struct _equippableStats {
    uint32_t armour = 0;
    uint32_t hp = 0;
    uint8_t speed = 0;
    uint8_t power = 0;
    uint8_t wisdom = 0;
} EquippableStats;

typedef struct _statsPair {
    std::string name;
    std::string value;
} StatsPair;

typedef struct _equipmentStats : public EquippableStats {
} EquipmentStats;

typedef struct _gearStats : public EquipmentStats {
} GearStats;

typedef struct _damageStats {
    uint8_t numDice = 0;
    uint8_t diceSize = 0;
    uint32_t flatDamage = 0;
    uint8_t power = 0;

    static std::string toString(const _damageStats& stats) {
        if(stats.numDice == 0) {
            return std::to_string(stats.flatDamage);
        }

        auto base = std::to_string(stats.numDice) + "D" + std::to_string(stats.diceSize);

        if(stats.flatDamage == 0) {
            return base;
        }

        if(stats.flatDamage < 0) {
            return base + std::to_string(stats.flatDamage);
        }

        return base + "+" + std::to_string(stats.flatDamage);
    }

    static bool isZero(const _damageStats& stats) {
        if(stats.power == 0) {
            return true;
        }

        if(stats.numDice == 0 && stats.flatDamage == 0) {
            return true;
        }

        if(stats.diceSize == 0 && stats.flatDamage == 0) {
            return true;
        }

        return false;
    }
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

    static std::vector<StatsPair> calculateStatsPairs(const _itemStats& stats) {
        std::vector<StatsPair> pairs;

        if(stats.gear.armour != 0)  pairs.push_back({ "Armour", std::to_string(stats.gear.armour) });
        if(stats.gear.hp != 0)      pairs.push_back({ "HP", std::to_string(stats.gear.hp) });
        if(stats.gear.speed != 0)   pairs.push_back({ "Speed", std::to_string(stats.gear.speed) });
        if(stats.gear.power != 0)   pairs.push_back({ "Power", std::to_string(stats.gear.power) });
        if(stats.gear.wisdom != 0)  pairs.push_back({ "Wisdom", std::to_string(stats.gear.wisdom) });

        if(!DamageStats2::isZero(stats.weapon.damage)) {
            pairs.push_back({ "Damage", DamageStats2::toString(stats.weapon.damage) });
        }

        if(stats.weapon.uses != 0)  pairs.push_back({ "Uses", std::to_string(stats.weapon.uses) });
        if(stats.weapon.range != 0) pairs.push_back({ "Range", std::to_string(stats.weapon.range) });

        return pairs;
    }
} ItemStats;
