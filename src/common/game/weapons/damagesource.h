#pragma once

#include <string>
#include <regex>
#include <iterator>
#include <stdexcept>

#include "core/util/randomrolls.h"
#include "game/entities/entity.h"

class Entity;

class DamageSource {
private:
    int numDice;
    int diceSize;
    int flatDamage;

    DamageSource();

    static bool isValid(const std::string& value);
    static void parseValues(const std::string& value, int& numDice, int& diceSize, int& flatDamage);
    static int getFlatDamageModifier(const std::string& value);

public:
    DamageSource(int numDice, int diceSize, int flatDamage);

    static DamageSource parse(const std::string& value);

    void apply(Entity* entity);

    int getNumDice(void) const;
    void setNumDice(int numDice);
    int getDiceSize(void) const;
    void setDiceSize(int diceSize);
    int getFlatDamage(void) const;
    void setFlatDamage(int flatDamage);

    std::string getString(void);
};
