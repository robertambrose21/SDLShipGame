#include "stats.h"

Stats::Stats() :
    moves(0),
    hp(0),
    armour(0)
{ }

Stats::Stats(int moves, int hp, int armour) :
    moves(moves),
    hp(hp),
    armour(armour)
{ }

void Stats::add(const Stats& other) {
    moves += other.moves;
    hp += other.hp;
    armour += other.armour;
}

void Stats::remove(const Stats& other) {
    moves -= other.moves;
    hp -= other.hp;
    armour -= other.armour;
}

std::map<std::string, std::string> Stats::getValues(void) {
    std::map<std::string, std::string> values;

    if(moves != 0) values["Moves"] = std::to_string(moves);
    if(hp != 0) values["HP"] = std::to_string(hp);
    if(armour != 0) values["Armour"] = std::to_string(armour);
    
    return values;
}

WeaponStats::WeaponStats(int range, int uses, int power) :
    Stats(),
    range(range),
    uses(uses),
    power(power)
{ }

void WeaponStats::add(const WeaponStats& other) {
    Stats::add(other);
    range += other.range;
    uses += other.uses;
    power += other.power;
}

void WeaponStats::remove(const WeaponStats& other) {
    Stats::remove(other);
    range -= other.range;
    uses -= other.uses;
    power -= other.power;
}

std::map<std::string, std::string> WeaponStats::getValues(void) {
    std::map<std::string, std::string> values = Stats::getValues();

    if(range != 0) values["Range"] = std::to_string(range);
    if(uses != 0) values["Uses"] = std::to_string(uses);
    if(power != 0) values["Power"] = std::to_string(power);

    return values;
}