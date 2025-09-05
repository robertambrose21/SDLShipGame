#pragma once

#include <cstdint>
#include <vector>
#include <map>
#include <string>
#include <format>

#include "game/effects/effecttypes.h"
#include "core/util/hashing.h"

namespace Stats {
    enum StatCategory {
        BASE,
        WEAPON,
        EFFECT,
        AREA_OF_EFFECT,
        ENTITY
    };

    struct Stats { 
        virtual std::size_t getHash(void) = 0;
    };

    typedef struct _statsPair {
        std::string name;
        std::string value;
    } StatsPair;

    // When adding new stats, ensure they have a default value set
    typedef struct _equipmentStats : public Stats {
        uint32_t armour = 0;
        uint32_t hp = 0;
        uint8_t speed = 0;
        uint8_t power = 0;
        uint8_t wisdom = 0;

        _equipmentStats() = default;
        _equipmentStats(
            uint32_t armour, 
            uint32_t hp, 
            uint8_t speed, 
            uint8_t power,
            uint8_t wisdom
        ) : 
            armour(armour), 
            hp(hp), 
            speed(speed), 
            power(power), 
            wisdom(wisdom)
        { }

        std::size_t getHash(void) {
            return constructHash(0, armour, hp, speed, power, wisdom);
        }
    } EquipmentStats;

    typedef struct _gearStats : public EquipmentStats {
        _gearStats() = default;
        _gearStats(
            uint32_t armour, 
            uint32_t hp, 
            uint8_t speed, 
            uint8_t power, 
            uint8_t wisdom
        ) : 
            EquipmentStats(armour, hp, speed, power, wisdom)
        { }
    } GearStats;

    typedef struct _damageStats : public Stats {
        uint8_t numDice = 0;
        uint8_t diceSize = 0;
        uint32_t flatDamage = 0;
        uint8_t power = 0;

        _damageStats() = default;
        _damageStats(
            uint8_t numDice, 
            uint8_t diceSize, 
            uint32_t flatDamage, 
            uint8_t power
        ) :
            numDice(numDice), 
            diceSize(diceSize), 
            flatDamage(flatDamage), 
            power(power)
        { }

        std::size_t getHash(void) {
            return constructHash(0, numDice, diceSize, flatDamage, power);
        }
    } DamageStats;

    typedef struct _aoeStats : public Stats {
        DamageStats damage;
        float radius = 0;
        uint8_t duration = 0;

        _aoeStats() = default;
        _aoeStats(
            const DamageStats& damage, 
            float radius, 
            uint8_t duration
        ) : 
            damage(damage), 
            radius(radius), 
            duration(duration)
        { }

        std::size_t getHash(void) {
            std::size_t h = 0;

            combineHashes(h, damage.getHash());

            return constructHash(h, radius, duration);
        }
    } AoEStats;

    typedef struct _effectStats : public Stats {
        EffectType type;
        uint8_t duration = 0;
        std::vector<uint32_t> damageTicks;

        _effectStats() = default;
        _effectStats(
            EffectType type, 
            uint8_t duration, 
            const std::vector<uint32_t>& damageTicks
        ) :
            type(type),
            duration(duration), 
            damageTicks(damageTicks)
        { }

        std::size_t getHash(void) {
            std::size_t h = 0;

            for (auto tick : damageTicks) {
                combineHashes(h, std::hash<uint32_t>{}(tick));
            }

            return constructHash(h, type, duration);
        }
    } EffectStats;

    typedef struct _projectileStats : public Stats {
        float speed;
        std::vector<EffectStats> effects;
        AoEStats aoe;

        _projectileStats() = default;
        _projectileStats(
            float speed, 
            const std::vector<EffectStats>& effects, 
            const AoEStats& aoe
        ) : 
            speed(speed), 
            effects(effects), 
            aoe(aoe)
        { }

        std::size_t getHash(void) {
            std::size_t h = 0;

            for (auto effect : effects) {
                combineHashes(h, effect.getHash());
            }

            combineHashes(h, aoe.getHash());

            return constructHash(h, speed);
        }
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

        _weaponStats() = default;
        _weaponStats(
            WeaponClass weaponClass, 
            const DamageStats& damage, 
            const ProjectileStats& projectile,
            uint8_t uses, 
            uint8_t range,
            uint32_t armour = 0, 
            uint32_t hp = 0, 
            uint8_t speed = 0, 
            uint8_t power = 0, 
            uint8_t wisdom = 0
        ) :
            EquipmentStats(armour, hp, speed, power, wisdom),
            weaponClass(weaponClass), 
            damage(damage), 
            projectile(projectile), 
            uses(uses), 
            range(range)
        { }

        std::size_t getHash(void) {
            std::size_t h = 0;

            combineHashes(h, EquipmentStats::getHash());
            combineHashes(h, damage.getHash());
            combineHashes(h, projectile.getHash());

            return constructHash(h, weaponClass, range);
        }
    } WeaponStats;

    typedef struct _actorStats : public EquipmentStats {
        uint32_t totalHp = 0;
        uint8_t movesPerTurn = 0;
        uint8_t movesLeft = 0;

        _actorStats() = default;
        _actorStats(
            uint32_t totalHp, 
            uint8_t movesPerTurn, 
            uint8_t movesLeft,
            uint32_t armour = 0, 
            uint32_t hp = 0, 
            uint8_t speed = 0, 
            uint8_t power = 0, 
            uint8_t wisdom = 0
        ) : 
            EquipmentStats(armour, hp, speed, power, wisdom),
            totalHp(totalHp), 
            movesPerTurn(movesPerTurn), 
            movesLeft(movesLeft)
        { }


        std::size_t getHash(void) {
            return constructHash(EquipmentStats::getHash(), movesPerTurn, movesLeft);
        }
    } ActorStats;

    typedef struct _itemStats : public Stats { 
        GearStats gear;
        WeaponStats weapon;

        std::size_t getHash(void) {
            std::size_t h = 0;

            combineHashes(h, gear.getHash());
            combineHashes(h, weapon.getHash());

            return h;
        }
    } ItemStats;

    static std::string getEffectLabel(const EffectStats& effectStats) {
        switch(effectStats.type) {
            case FREEZE:
                return "Freeze";
            case POISON:
                return "Poison";
            default:
                return "!Unknown Effect!";
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

    static std::map<StatCategory, std::vector<StatsPair>> calculateActorStatCategories(const ActorStats& stats) {
        std::map<StatCategory, std::vector<StatsPair>> categories;

        categories[ENTITY].push_back({ "HP", std::format("{}/{}", stats.hp, stats.totalHp) });
        categories[ENTITY].push_back({ "Moves", std::format("{}/{}", stats.movesLeft, stats.movesPerTurn) });

        categories.merge(calculateBaseStatCategories(stats));

        std::erase_if(categories[BASE], [](const auto& item) {
            return item.name == "HP";
        });

        return categories;
    }

    static void addEquipmentStatsToActor(ActorStats& actorStats, const EquipmentStats& equipment) {
        actorStats.hp += equipment.hp;
        actorStats.totalHp += equipment.hp;

        actorStats.armour += equipment.armour;
        actorStats.power += equipment.power;
        actorStats.speed += equipment.speed;
        actorStats.wisdom += equipment.wisdom;

        // Temp calculation, needs balancing
        actorStats.movesPerTurn += equipment.speed / 5;
        actorStats.movesLeft += equipment.speed / 5;
    }
}
