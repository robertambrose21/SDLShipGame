#include "damagesource.h"
#include "game/entities/entity.h" // Fuck you C++ circular dependency shenanigans

DamageSource::DamageSource()
{ }


DamageSource::DamageSource(const DamageStats& stats) :
    stats(stats)
{ }

DamageSource DamageSource::parse(const std::string& value, int power) {
    if(!isValid(value)) {
        throw std::runtime_error("failed regex");
    }

    DamageSource damageSource;

    parseValues(value, damageSource.stats.numDice, damageSource.stats.diceSize, damageSource.stats.flatDamage);

    damageSource.stats.power = power;

    return damageSource;
}

bool DamageSource::isValid(const std::string& value) {
    auto regex = std::regex("(([0-9]+)D([0-9]+)((\\+|-)([0-9]+))*)|^([0-9]+)");
    return std::regex_match(value, regex);
}

void DamageSource::parseValues(const std::string& value, int& numDice, int& diceSize, int& flatDamage) {
    auto regex = std::regex("[0-9]+");
    auto begin = std::sregex_iterator(value.begin(), value.end(), regex);
    auto end = std::sregex_iterator();

    auto numValues = std::distance(begin, end);

    if(numValues == 1) {
        flatDamage = std::stoi((*begin).str());
        return;
    }

    numDice = std::stoi((*begin++).str());

    if(numValues > 1) {
        diceSize = std::stoi((*begin++).str());
    }
    if(numValues > 2) {
        auto modifier = getFlatDamageModifier(value);
        flatDamage = std::stoi((*begin++).str()) * modifier;
    }
}

int DamageSource::getFlatDamageModifier(const std::string& value) {
    auto regex = std::regex("(\\+|-)");
    std::smatch match;

    if(std::regex_search(value, match, regex)) {
        auto modifier = match.str()[0];

        if(modifier == '+') {
            return 1;
        }
        if(modifier == '-') {
            return -1;
        }
    }

    throw std::runtime_error("failed regex");
}

int DamageSource::apply(Entity* entity) {
    int damage = stats.flatDamage;

    for(int i = 0; i < stats.numDice; i++) {
        damage += randomDN(stats.diceSize);
    }

    damage *= (stats.power / entity->getStats().armour);

    entity->takeDamage(damage);

    return damage;
}

DamageStats DamageSource::getStats(void) const {
    return stats;
}

bool DamageSource::isZero(void) {
    return stats.isZero();
}

std::string DamageSource::getDamageString(void) {
    return stats.getDamageString();
}