#pragma once

// TODO: Add 'currentHP' and change 'hp' to 'totalHP'.
// TODO: Pass in both base stats and current stats to status effect apply - appropriate columns
typedef struct _entityBaseStats {
    int movesPerTurn;
    int totalHP;
} EntityBaseStats;

typedef struct _entityCurrentStats : EntityBaseStats {
    int currentHP;
    int movesLeft;

    _entityCurrentStats(const EntityBaseStats& baseStats) :
        EntityBaseStats(baseStats),
        currentHP(baseStats.totalHP),
        movesLeft(0)
    { }
} EntityCurrentStats;