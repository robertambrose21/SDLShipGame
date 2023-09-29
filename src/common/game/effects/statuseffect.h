#pragma once

#include <set>

#include "game/entities/entitystats.h"

enum BaseStat {
    MovesPerTurn,
    TotalHP
};

enum FluidStat {
    MovesLeft,
    CurrentHP
};

// Should be able to:
// - Modify current stats for duration of effect, either +, - or *
//   - e.g. hp - 1, each turn - take 1 off hp
// - Scale per turn, e.g. 1 hp turn 1, 2 hp turn 2, etc.
// - Apply a new status effect on certain events (e.g. on entity death, specific turn)
class StatusEffect {
public:
    typedef struct _modifier {
        double value;
        char operation;
        BaseStat statName;
    } Modifier;

private:
    std::set<Modifier> modifiers;
    int turnsActive;
    int turnsLeft;

    double modify(double currentValue, double modifierValue, char operation);

public:
    StatusEffect(int turnsActive);

    void addModifier(const Modifier& modifier);
    void apply(const EntityBaseStats& baseStats);

    int getTurnsLeft(void) const;
};

inline bool operator<(const StatusEffect::Modifier& lhs, const StatusEffect::Modifier& rhs) {
    return lhs.value < rhs.value;
}