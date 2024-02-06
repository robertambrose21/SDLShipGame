#include "damagesource.h"

DamageSource::DamageSource() :
    numDice(0),
    diceSize(0),
    flatDamage(0)
{ }


DamageSource::DamageSource(int numDice, int diceSize, int flatDamage) :
    numDice(numDice),
    diceSize(diceSize),
    flatDamage(flatDamage)
{
    game_assert(numDice >= 0);
    game_assert(diceSize > 0);
}

DamageSource DamageSource::parse(const std::string& value) {
    if(!isValid(value)) {
        throw std::runtime_error("failed regex");
    }

    DamageSource damageSource;

    parseValues(value, damageSource.numDice, damageSource.diceSize, damageSource.flatDamage);

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

void DamageSource::apply(Entity* entity) {
    int damage = flatDamage;

    for(int i = 0; i < numDice; i++) {
        damage += randomDN(diceSize);
    }

    entity->takeDamage(damage);
}

int DamageSource::getNumDice(void) const {
    return numDice;
}

void DamageSource::setNumDice(int numDice) {
    this->numDice = numDice;
}

int DamageSource::getDiceSize(void) const {
    return diceSize;
}

void DamageSource::setDiceSize(int diceSize) {
    this->diceSize = diceSize;
}

int DamageSource::getFlatDamage(void) const {
    return flatDamage;
}

void DamageSource::setFlatDamage(int flatDamage) {
    this->flatDamage = flatDamage;
}

std::string DamageSource::getString(void) {
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