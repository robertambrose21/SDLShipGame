#pragma once

#include "game/items/item.h"
#include "game/stats/equippablestats.h"

template<typename T>
class Equippable {
public:
    enum Slot {
        WEAPON_1,
        WEAPON_2,
        HEAD,
        BODY,
        HANDS,
        FEET,
        COUNT
    };

    virtual Slot getSlot(void) = 0;
    virtual void setSlot(Slot slot) = 0;
    virtual Item* getItem(void) = 0;
    virtual bool isValid(void) = 0;
    
    virtual T getStats(void) = 0;
    virtual void addTo(EntityStats& entityStats) = 0;
};
