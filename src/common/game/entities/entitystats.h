#pragma once

// TODO: Add 'currentHP' and change 'hp' to 'totalHP'.
// TODO: Pass in both base stats and current stats to status effect apply - appropriate columns
typedef struct _entityBaseStats {
    int movesPerTurn;
    int totalHP;
    int baseArmour;
} EntityBaseStats;

typedef struct _entityCurrentStats : EntityBaseStats {
    int currentHP;
    int movesLeft;

    _entityCurrentStats()
    { }
    
    _entityCurrentStats(const EntityBaseStats& baseStats) :
        EntityBaseStats(baseStats),
        currentHP(baseStats.totalHP),
        movesLeft(baseStats.movesPerTurn)
    { }
} EntityCurrentStats;