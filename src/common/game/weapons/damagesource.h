#pragma once

#include <string>
#include <regex>
#include <iterator>
#include <stdexcept>

#include "core/util/randomrolls.h"
#include "game/stats/stats.h"

class Entity;

class DamageSource {
private:
    DamageStats stats;

    static bool isValid(const std::string& value);
    static void parseValues(const std::string& value, int& numDice, int& diceSize, int& flatDamage);
    static int getFlatDamageModifier(const std::string& value);

public:
    DamageSource();
    DamageSource(const DamageStats& stats);

    static DamageSource parse(const std::string& value, int power);

    int apply(Entity* entity);

    DamageStats getStats(void) const;
    bool isZero(void);
    std::string getDamageString(void);
};
