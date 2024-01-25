#pragma once

#include <map>
#include <string>

#include "game/items/item.h"
#include "core/util/gameassert.h"

class Equipment {
public:
    enum Slot {
        HEAD,
        BODY,
        HANDS,
        FEET,
        COUNT
    };

private:
    static const std::map<Slot, std::string> SlotToItemType;

    Item* item;
    Slot slot;

public:
    Equipment();
    Equipment(Item* item, Slot slot);

    Item* getItem(void);
    
    Slot getSlot(void);
    void setSlot(Slot slot);

    static bool isValid(Item* item, Slot slot);
};