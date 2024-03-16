#include "stats.h"

bool operator==(StatsKey const& lhs, StatsKey const& rhs) {
    return lhs.keyName == rhs.keyName && lhs.type == rhs.type;
}

bool operator<(StatsKey const& lhs, StatsKey const& rhs) {
    return lhs.keyName < rhs.keyName || (lhs.keyName == rhs.keyName && lhs.type < rhs.type);
}

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

std::map<StatsKey, std::string> CommonStats::getValues(void) {
    std::map<StatsKey, std::string> values;

    if(moves != 0) values[{ "Moves", StatsKey::Common }] = std::to_string(moves);
    if(hp != 0) values[{ "HP", StatsKey::Common }] = std::to_string(hp);
    if(armour != 0) values[{ "Armour", StatsKey::Common }] = std::to_string(armour);
    
    return values;
}

DamageStats::DamageStats() :
    numDice(0),
    diceSize(0),
    flatDamage(0),
    power(0)
{ }

DamageStats::DamageStats(int numDice, int diceSize, int flatDamage, int power) :
    numDice(numDice),
    diceSize(diceSize),
    flatDamage(flatDamage),
    power(power)
{ }

void DamageStats::add(const DamageStats& other) {
    numDice += other.numDice;
    diceSize += other.diceSize;
    flatDamage += other.flatDamage;
    power += other.power;
}

void DamageStats::remove(const DamageStats& other) {
    numDice -= other.numDice;
    diceSize -= other.diceSize;
    flatDamage -= other.flatDamage;
    power -= other.power;
}

std::string DamageStats::getDamageString(void) {
    if(numDice == 0) {
        return std::to_string(flatDamage);
    }

    auto base = std::to_string(numDice) + "D" + std::to_string(diceSize);

    if(flatDamage == 0) {
        return base;
    }

    if(flatDamage < 0) {
        return base + std::to_string(flatDamage);
    }

    return base + "+" + std::to_string(flatDamage);
}

bool DamageStats::isZero(void) {
    if(power == 0) {
        return true;
    }

    if(numDice == 0 && flatDamage == 0) {
        return true;
    }

    if(diceSize == 0 && flatDamage == 0) {
        return true;
    }

    return false;
}

std::map<StatsKey, std::string> DamageStats::getValues(void) {
    std::map<StatsKey, std::string> values;

    if(!isZero()) values[{ "Damage", StatsKey::Damage }] = getDamageString();
    if(power != 0) values[{ "Power", StatsKey::Damage }] = std::to_string(power);

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

std::map<StatsKey, std::string> EffectStats::getValues(void) {
    std::map<StatsKey, std::string> values;

    switch(type) {
        case FREEZE:
            values[{ "Type", StatsKey::Effect }] = "Freeze";
            break;

        case POISON:
            values[{ "Type", StatsKey::Effect }] = "Poison";
            break;

        default:
            break;
    }

    if(duration != 0) values[{ "Duration", StatsKey::Effect }] = std::to_string(duration);
    if(!damageTicks.empty()) values[{ "Damage per tick", StatsKey::Effect }] = std::to_string(damageTicks[0]);

    return values;
}

bool EffectStats::operator==(const EffectStats& other) {
    if(type != other.type || duration != other.duration) {
        return false;
    }

    if(damageTicks.size() != other.damageTicks.size()) {
        return false;
    }

    for(int i = 0; i < damageTicks.size(); i++) {
        if(damageTicks[i] != other.damageTicks[i]) {
            return false;
        }
    }

    return true;
}

AreaOfEffectStats::AreaOfEffectStats() :
    radius(0),
    turns(0)
{ }

AreaOfEffectStats::AreaOfEffectStats(float radius, int turns, const DamageStats& damage) :
    radius(radius),
    turns(turns),
    damage(damage)
{ }

void AreaOfEffectStats::add(const AreaOfEffectStats& other) {
    radius += other.radius;
    turns += other.turns;
    damage.add(other.damage);
}

void AreaOfEffectStats::remove(const AreaOfEffectStats& other) {
    radius -= other.radius;
    turns -= other.turns;
    damage.remove(other.damage);
}

std::map<StatsKey, std::string> AreaOfEffectStats::getValues(void) {
    std::map<StatsKey, std::string> values;

    if(radius != 0) values[{ "Radius", StatsKey::AreaOfEffect }] = std::to_string(radius);
    if(turns != 0) values[{ "Turns", StatsKey::AreaOfEffect }] = std::to_string(turns);
    if(damage.power != 0) values[{ "Power", StatsKey::AreaOfEffect }] = std::to_string(damage.power);
    if(!damage.isZero()) values[{ "Damage", StatsKey::AreaOfEffect }] = damage.getDamageString();

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
        if(std::find(effects.begin(), effects.end(), effect) == effects.end()) {
            effects.push_back(effect);
        }
    }

    aoe.add(other.aoe);
}

void ProjectileStats::remove(const ProjectileStats& other) {
    speed -= other.speed;
    aoe.remove(other.aoe);
    
    for(auto otherEffect : other.effects) {
        std::erase_if(effects, [&](const auto& effect) {
            return effect == otherEffect;
        });
    }
}

std::map<StatsKey, std::string> ProjectileStats::getValues(void) {
    std::map<StatsKey, std::string> values;

    if(speed != 0) values[{ "Speed", StatsKey::Projectile }] = std::to_string(speed);

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
    uses(0)
{ }

WeaponStats::WeaponStats(int range, int uses, const DamageStats& damage) :
    range(range),
    uses(uses),
    damage(damage)
{ }

void WeaponStats::add(const WeaponStats& other) {
    range += other.range;
    uses += other.uses;
    damage.add(other.damage);
    projectile.add(other.projectile);
}

void WeaponStats::remove(const WeaponStats& other) {
    range -= other.range;
    uses -= other.uses;
    damage.remove(other.damage);
    projectile.remove(other.projectile);
}

std::map<StatsKey, std::string> WeaponStats::getValues(void) {
    std::map<StatsKey, std::string> values;

    if(range != 0) values[{ "Range", StatsKey::Weapon }] = std::to_string(range);
    if(uses != 0) values[{ "Uses", StatsKey::Weapon }] = std::to_string(uses);
    if(damage.power != 0) values[{ "Power", StatsKey::Weapon }] = std::to_string(damage.power);
    if(!damage.isZero()) values[{ "Damage", StatsKey::Weapon }]  = damage.getDamageString();

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

std::map<StatsKey, std::string> AllStats::getValues(void) {
    std::map<StatsKey, std::string> values;

    auto commonValues = common.getValues();
    auto weaponValues = weapon.getValues();

    values.insert(commonValues.begin(), commonValues.end());
    values.insert(weaponValues.begin(), weaponValues.end());

    return values;
}