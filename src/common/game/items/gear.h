#pragma once

#include <array>

#include "equippable.h"

class Gear : Equippable<GearStats> {
private:
    Slot slot;
    Item* item;

public:
    static constexpr std::array<Slot, 4> VALID_SLOTS = { HEAD, BODY, HANDS, FEET };

    Gear(Slot slot, Item* item);

    Slot getSlot(void);
    void setSlot(Slot slot);
    Item* getItem(void);
    bool isValid(void);
    GearStats getStats(void);
    void addTo(EntityStats& entityStats);
};
