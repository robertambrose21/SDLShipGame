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

std::map<std::string, std::string> CommonStats::getValues(void) {
    std::map<std::string, std::string> values;

    if(moves != 0) values["Moves"] = std::to_string(moves);
    if(hp != 0) values["HP"] = std::to_string(hp);
    if(armour != 0) values["Armour"] = std::to_string(armour);
    
    return values;
}

WeaponStats::WeaponStats() :
    range(0),
    uses(0),
    power(0)
{ }

WeaponStats::WeaponStats(int range, int uses, int power) :
    range(range),
    uses(uses),
    power(power)
{ }

void WeaponStats::add(const WeaponStats& other) {
    range += other.range;
    uses += other.uses;
    power += other.power;
}

void WeaponStats::remove(const WeaponStats& other) {
    range -= other.range;
    uses -= other.uses;
    power -= other.power;
}

std::map<std::string, std::string> WeaponStats::getValues(void) {
    std::map<std::string, std::string> values;

    if(range != 0) values["Range"] = std::to_string(range);
    if(uses != 0) values["Uses"] = std::to_string(uses);
    if(power != 0) values["Power"] = std::to_string(power);

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

std::map<std::string, std::string> AllStats::getValues(void) {
    std::map<std::string, std::string> values;

    auto commonValues = common.getValues();
    auto weaponValues = weapon.getValues();

    values.insert(commonValues.begin(), commonValues.end());
    values.insert(weaponValues.begin(), weaponValues.end());

    return values;
}