#pragma once

#include <map>
#include <string>
#include <vector>
#include <algorithm>

#include "game/effects/effecttypes.h"
#include "game/weapons/damagesource.h"
#include "core/util/gameassert.h"

typedef struct _statsKey {
    enum Type {
        Common,
        Damage,
        Effect,
        AreaOfEffect,
        Projectile,
        Weapon,
        All
    };

    std::string keyName;
    Type type;
} StatsKey;

bool operator==(StatsKey const& lhs, StatsKey const& rhs);
bool operator<(StatsKey const& lhs, StatsKey const& rhs);

template<class T>
class Stats {
public:
    virtual void add(const T& other) = 0;
    virtual void remove(const T& other) = 0;

    virtual std::map<StatsKey, std::string> getValues(void) = 0;
};

class CommonStats : Stats<CommonStats> {
public:
    int moves;
    int hp;
    int armour;

    CommonStats();
    CommonStats(int moves, int hp, int armour);

    void add(const CommonStats& other);
    void remove(const CommonStats& other);

    std::map<StatsKey, std::string> getValues(void);
};

class DamageStats : Stats<DamageStats> {
public:
    int numDice;
    int diceSize;
    int flatDamage;
    int power;

    DamageStats();
    DamageStats(int numDice, int diceSize, int flatDamage, int power);

    void add(const DamageStats& other);
    void remove(const DamageStats& other);

    std::string getDamageString(void);
    bool isZero(void);

    std::map<StatsKey, std::string> getValues(void);
};

class EffectStats : Stats<EffectStats> {
public:
    EffectType type;
    int duration;
    std::vector<int> damageTicks;

    EffectStats();
    EffectStats(EffectType type, int duration, const std::vector<int>& damageTicks);

    void add(const EffectStats& other);
    void remove(const EffectStats& other);

    std::map<StatsKey, std::string> getValues(void);

    bool operator==(const EffectStats& other);
};

class AreaOfEffectStats : Stats<AreaOfEffectStats> {
public:
    float radius;
    int turns;
    int power;
    DamageSource damageSource;

    AreaOfEffectStats();
    AreaOfEffectStats(float radius, int turns, int power, const DamageSource& damageSource);

    void add(const AreaOfEffectStats& other);
    void remove(const AreaOfEffectStats& other);

    std::map<StatsKey, std::string> getValues(void);
};

class ProjectileStats : Stats<ProjectileStats> {
public:
    float speed;
    std::vector<EffectStats> effects;
    AreaOfEffectStats aoe;

    ProjectileStats();
    ProjectileStats(float speed, const std::vector<EffectStats>& effects);

    void add(const ProjectileStats& other);
    void remove(const ProjectileStats& other);

    std::map<StatsKey, std::string> getValues(void);
};

class WeaponStats : Stats<WeaponStats> {
public:
    int range;
    int uses;
    int power;
    DamageSource damageSource;
    ProjectileStats projectile;

    WeaponStats();
    WeaponStats(int range, int uses, int power, const DamageSource& damageSource);

    void add(const WeaponStats& other);
    void remove(const WeaponStats& other);

    std::map<StatsKey, std::string> getValues(void);
};

class AllStats : Stats<AllStats> {
public:
    CommonStats common;
    WeaponStats weapon;

    AllStats();

    void add(const AllStats& other);
    void remove(const AllStats& other);

    std::map<StatsKey, std::string> getValues(void);
};
