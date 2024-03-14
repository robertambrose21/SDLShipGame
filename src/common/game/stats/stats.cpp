#include "stats.h"

CommonStats::CommonStats() :
    moves(0),
    hp(0),
    armour(0)
{ }

CommonStats::CommonStats(int moves, int hp, int armour) :
    moves(moves),
    hp(hp),
    armour(armour)
{ }

void CommonStats::add(const CommonStats& other) {
    moves += other.moves;
    hp += other.hp;
    armour += other.armour;
}

void CommonStats::remove(const CommonStats& other) {
    moves -= other.moves;
    hp -= other.hp;
    armour -= other.armour;
}

std::map<std::string, StatsValue> CommonStats::getValues(void) {
    std::map<std::string, StatsValue> values;

    if(moves != 0) values["Moves"] = { std::to_string(moves), StatsValue::Common };
    if(hp != 0) values["HP"] = { std::to_string(hp), StatsValue::Common };
    if(armour != 0) values["Armour"] = { std::to_string(armour), StatsValue::Common };
    
    return values;
}

EffectStats::EffectStats() :
    duration(0)
{ }

EffectStats::EffectStats(EffectType type, int duration, const std::vector<int>& damageTicks) :
    type(type),
    duration(duration),
    damageTicks(damageTicks)
{
    // TODO: Proper handling
    game_assert(damageTicks.size() <= duration);
}

void EffectStats::add(const EffectStats& other) {
    if(type != other.type) {
        return;
    }

    duration += other.duration;
}

void EffectStats::remove(const EffectStats& other) {
    if(type != other.type) {
        return;
    }

    duration -= other.duration;
}

std::map<std::string, StatsValue> EffectStats::getValues(void) {
    std::map<std::string, StatsValue> values;

    switch(type) {
        case FREEZE:
            values["Type"] = { "Freeze", StatsValue::Effect };
            break;

        case POISON:
            values["Type"] = { "Poison", StatsValue::Effect };
            break;

        default:
            break;
    }

    if(duration != 0) values["Duration"] = { std::to_string(duration), StatsValue::Effect };
    if(!damageTicks.empty()) values["Damage per tick"] = { std::to_string(damageTicks[0]), StatsValue::Effect };

    return values;
}

AreaOfEffectStats::AreaOfEffectStats() :
    radius(0),
    turns(0),
    power(0)
{ }

AreaOfEffectStats::AreaOfEffectStats(float radius, int turns, int power, const DamageSource& damageSource) :
    radius(radius),
    turns(turns),
    power(power),
    damageSource(damageSource)
{ }

void AreaOfEffectStats::add(const AreaOfEffectStats& other) {
    radius += other.radius;
    turns += other.turns;
    power += other.power;
    damageSource = other.damageSource;
}

void AreaOfEffectStats::remove(const AreaOfEffectStats& other) {
    radius -= other.radius;
    turns -= other.turns;
    power -= other.power;
}

std::map<std::string, StatsValue> AreaOfEffectStats::getValues(void) {
    std::map<std::string, StatsValue> values;

    if(radius != 0) values["Radius"] = { std::to_string(radius), StatsValue::AreaOfEffect };
    if(turns != 0) values["Turns"] = { std::to_string(turns), StatsValue::AreaOfEffect };
    if(power != 0) values["AoE Power"] = { std::to_string(power), StatsValue::AreaOfEffect };
    if(!damageSource.isZero()) values["AoE Damage"]  = { damageSource.getDamageString(), StatsValue::AreaOfEffect };

    return values;
}

ProjectileStats::ProjectileStats() :
    speed(0.0f)
{ }

ProjectileStats::ProjectileStats(float speed, const std::vector<EffectStats>& effects) :
    speed(speed),
    effects(effects)
{ }

void ProjectileStats::add(const ProjectileStats& other) {
    speed += other.speed;

    for(auto effect : other.effects) {
        effects.push_back(effect);
    }

    aoe.add(other.aoe);
}

void ProjectileStats::remove(const ProjectileStats& other) {
    speed -= other.speed;
    aoe.remove(other.aoe);
    // Deliberately do not remove effects
}

std::map<std::string, StatsValue> ProjectileStats::getValues(void) {
    std::map<std::string, StatsValue> values;

    if(speed != 0) values["Speed"] = { std::to_string(speed), StatsValue::Projectile };

    for(auto effect : effects) {
        auto effectValues = effect.getValues();
        values.insert(effectValues.begin(), effectValues.end());
    }

    auto aoeValues = aoe.getValues();
    values.insert(aoeValues.begin(), aoeValues.end());

    return values;
}

WeaponStats::WeaponStats() :
    range(0),
    uses(0),
    power(0)
{ }

WeaponStats::WeaponStats(int range, int uses, int power, const DamageSource& damageSource) :
    range(range),
    uses(uses),
    power(power),
    damageSource(damageSource)
{ }

void WeaponStats::add(const WeaponStats& other) {
    range += other.range;
    uses += other.uses;
    power += other.power;
    damageSource = other.damageSource;
    projectile.add(other.projectile);
}

void WeaponStats::remove(const WeaponStats& other) {
    range -= other.range;
    uses -= other.uses;
    power -= other.power;
    projectile.remove(other.projectile);
}

std::map<std::string, StatsValue> WeaponStats::getValues(void) {
    std::map<std::string, StatsValue> values;

    if(range != 0) values["Range"] = { std::to_string(range), StatsValue::Weapon };
    if(uses != 0) values["Uses"] = { std::to_string(uses), StatsValue::Weapon };
    if(power != 0) values["Weapon Power"] = { std::to_string(power), StatsValue::Weapon };
    if(!damageSource.isZero()) values["Weapon Damage"]  = { damageSource.getDamageString(), StatsValue::Weapon };

    auto projectileValues = projectile.getValues();
    values.insert(projectileValues.begin(), projectileValues.end());

    return values;
}

AllStats::AllStats()
{ }

void AllStats::add(const AllStats& other) {
    common.add(other.common);
    weapon.add(other.weapon);
}

void AllStats::remove(const AllStats& other) {
    common.remove(other.common);
    weapon.remove(other.weapon);
}

std::map<std::string, StatsValue> AllStats::getValues(void) {
    std::map<std::string, StatsValue> values;

    auto commonValues = common.getValues();
    auto weaponValues = weapon.getValues();

    values.insert(commonValues.begin(), commonValues.end());
    values.insert(weaponValues.begin(), weaponValues.end());

    return values;
}