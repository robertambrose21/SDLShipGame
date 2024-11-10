#pragma once

#include <cstdint>
#include <vector>
#include <map>
#include <string>
#include <format>

#include "game/effects/effecttypes.h"

namespace Stats {
    enum StatCategory {
        BASE,
        WEAPON,
        EFFECT,
        AREA_OF_EFFECT,
        ENTITY
    };

    typedef struct _statsPair {
        std::string name;
        std::string value;
    } StatsPair;

    // When adding new stats, ensure they have a default value set
    typedef struct _equipmentStats {
        uint32_t armour = 0;
        uint32_t hp = 0;
        uint8_t speed = 0;
        uint8_t power = 0;
        uint8_t wisdom = 0;
    } EquipmentStats;

    typedef struct _gearStats : public EquipmentStats {
    } GearStats;

    typedef struct _damageStats {
        uint8_t numDice = 0;
        uint8_t diceSize = 0;
        uint32_t flatDamage = 0;
        uint8_t power = 0;
    } DamageStats;

    typedef struct _aoeStats {
        DamageStats damage;
        float radius = 0;
        uint8_t duration = 0;
    } AoEStats;

    typedef struct _effectStats {
        EffectType type;
        uint8_t duration = 0;
        std::vector<uint32_t> damageTicks;
    } EffectStats;

    typedef struct _projectileStats {
        float speed;
        std::vector<EffectStats> effects;
        AoEStats aoe;
    } ProjectileStats;

    typedef struct _weaponStats : public EquipmentStats {
        enum WeaponClass {
            MELEE,
            PROJECTILE
        };

        WeaponClass weaponClass;
        DamageStats damage;
        ProjectileStats projectile;
        uint8_t uses = 0;
        uint8_t range = 0;
    } WeaponStats;

    typedef struct _entityStats : public EquipmentStats {
        uint32_t totalHp = 0;
        uint8_t movesPerTurn = 0;
        uint8_t movesLeft = 0;
    } EntityStats;

    typedef struct _itemStats { 
        GearStats gear;
        WeaponStats weapon;
    } ItemStats;

    static std::string getEffectLabel(const EffectStats& effectStats) {
        switch(effectStats.type) {
            case FREEZE:
                return "Freeze";
            case POISON:
                return "Poison";
            default:
                return "!Uknown Effect!";
        }
    }

    static std::string getEffectValue(const EffectStats& effectStats) {
        if(effectStats.damageTicks.empty()) {
            return std::format("for {} turns", effectStats.duration);
        }

        std::string ticks = "";

        for(int i = 0; i < effectStats.damageTicks.size(); i++) {
            ticks += std::to_string(effectStats.damageTicks[i]);

            if(i < effectStats.damageTicks.size() - 1) {
                ticks += ", ";
            }
        }

        return std::format("[{}] damage over {} turns", ticks, effectStats.duration);
    }

    static std::string statCategoryToString(StatCategory category) {
        switch(category) {
            case BASE:
                return "Base";
            case WEAPON:
                return "Weapon";
            case EFFECT:
                return "Effect";
            case AREA_OF_EFFECT:
                return "AoE";
            default:
                return std::format("!Unknown! ({})", (int) category);
        }
    }

    static std::string getDamageValue(const _damageStats& stats) {
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

    static bool isDamageZero(const _damageStats& stats) {
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

    static std::map<StatCategory, std::vector<StatsPair>> calculateBaseStatCategories(const EquipmentStats& stats) {
        std::map<StatCategory, std::vector<StatsPair>> categories;

        if(stats.armour != 0)  categories[BASE].push_back({ "Armour", std::to_string(stats.armour) });
        if(stats.hp != 0)      categories[BASE].push_back({ "HP", std::to_string(stats.hp) });
        if(stats.speed != 0)   categories[BASE].push_back({ "Speed", std::to_string(stats.speed) });
        if(stats.power != 0)   categories[BASE].push_back({ "Power", std::to_string(stats.power) });
        if(stats.wisdom != 0)  categories[BASE].push_back({ "Wisdom", std::to_string(stats.wisdom) });

        return categories;
    }

    static std::map<StatCategory, std::vector<StatsPair>> calculateItemStatCategories(const ItemStats& stats) {
        std::map<StatCategory, std::vector<StatsPair>> categories = calculateBaseStatCategories(stats.gear);

        if(!isDamageZero(stats.weapon.damage)) {
            std::string label = stats.weapon.weaponClass == WeaponStats::MELEE ? "Damage" : "Projectile Damage";
            categories[WEAPON].push_back({ label, getDamageValue(stats.weapon.damage) });
        }

        for(auto const& effect : stats.weapon.projectile.effects) {
            categories[EFFECT].push_back({ getEffectLabel(effect), getEffectValue(effect) });
        }

        if(!isDamageZero(stats.weapon.projectile.aoe.damage)) {
            categories[AREA_OF_EFFECT].push_back({ "Damage", getDamageValue(stats.weapon.projectile.aoe.damage) });
        }

        if(stats.weapon.projectile.aoe.duration > 1) {
            categories[AREA_OF_EFFECT].push_back({ "Duration", std::to_string(stats.weapon.projectile.aoe.duration) });
        }
        if(stats.weapon.projectile.aoe.radius != 0) {
            categories[AREA_OF_EFFECT].push_back({ "Radius", std::to_string(stats.weapon.projectile.aoe.radius) });
        }

        if(stats.weapon.uses != 0)  categories[WEAPON].push_back({ "Uses", std::to_string(stats.weapon.uses) });
        if(stats.weapon.range != 0) categories[WEAPON].push_back({ "Range", std::to_string(stats.weapon.range) });

        return categories;
    }

    static void addEquipmentStatsToEntity(EntityStats& entityStats, const EquipmentStats& equipment) {
        entityStats.hp += equipment.hp;
        entityStats.totalHp += equipment.hp;

        entityStats.armour += equipment.armour;
        entityStats.power += equipment.power;
        entityStats.speed += equipment.speed;
        entityStats.wisdom += equipment.wisdom;

        // Temp calculation, needs balancing
        entityStats.movesPerTurn += equipment.speed / 5;
        entityStats.movesLeft += equipment.speed / 5;
    }
}
