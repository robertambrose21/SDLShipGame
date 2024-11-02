#pragma once

#include <cstdint>

// When adding new stats, ensure they have a default value set
typedef struct _equippableStats {
    uint32_t armour = 0;
    uint32_t hp = 0;
    uint8_t speed = 0;
    uint8_t power = 0;
    uint8_t wisdom = 0;
} EquippableStats;

typedef struct _equipmentStats : public EquippableStats {
} EquipmentStats;

typedef struct _gearStats : public EquipmentStats {
} GearStats;

typedef struct _weaponStats : public EquipmentStats {
    uint8_t weaponPower = 0;
} WeaponStats2;

typedef struct _entityStats : public EquipmentStats {
    uint32_t totalHp = 0;
    uint8_t movesPerTurn = 0;
    uint8_t movesLeft = 0;
} EntityStats;

typedef struct _itemStats { 
    GearStats gear;
    WeaponStats2 weapon;
} ItemStats;
