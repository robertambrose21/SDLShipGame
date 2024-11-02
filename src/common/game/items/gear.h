#pragma once

#include <array>

#include "equippable.h"

class Gear : Equippable<GearStats> {
private:
    static constexpr std::array<Slot, 4> VALID_GEAR_SLOTS = { HEAD, BODY, HANDS, FEET };

    Slot slot;
    GearStats stats;
    Item* item;

public:
    Gear(Slot slot, const GearStats& stats, Item* item);

    Slot getSlot(void);
    void setSlot(Slot slot);
    Item* getItem(void);
    bool isValid(void);
    GearStats getStats(void);
    void addTo(EntityStats& entityStats);
};
