#pragma once

#include <cstdint>

typedef struct _equippableStats {
    uint32_t armour;
    uint32_t hp;
    uint8_t speed;
    uint8_t power;
    uint8_t wisdom;
} EquippableStats;

typedef struct _gearStats : public EquippableStats {
} GearStats;

typedef struct _weaponStats : public EquippableStats {
    uint8_t weaponPower;
} WeaponStats2;

typedef struct _entityStats : public EquippableStats {
    uint8_t movesPerTurn;
} EntityStats;
