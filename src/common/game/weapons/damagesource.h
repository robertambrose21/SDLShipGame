#pragma once

#include <string>
#include <regex>
#include <iterator>
#include <stdexcept>

#include "core/util/randomutils.h"
#include "game/stats/stats.h"

class Entity;

class DamageSource {
private:
    DamageStats2 stats;

    static bool isValid(const std::string& value);
    static void parseValues(const std::string& value, uint8_t& numDice, uint8_t& diceSize, uint32_t& flatDamage);
    static int getFlatDamageModifier(const std::string& value);

public:
    DamageSource();
    DamageSource(const DamageStats2& stats);

    static DamageSource parse(const std::string& value, uint8_t power);

    int apply(Entity* entity);

    DamageStats2 getStats(void) const;
};
