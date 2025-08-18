#pragma once

#include <array>
#include <string>

#include "game/items/item.h"
#include "game/stats/stats.h"

template<typename T>
class Equippable {
public:
    enum Slot {
        UNDEFINED,
        WEAPON_1,
        WEAPON_2,
        HEAD,
        BODY,
        HANDS,
        FEET,
        COUNT
    };

    static constexpr std::array<std::string, Slot::COUNT> SLOT_NAMES = {
        "Undefined",
        "Weapon 1", 
        "Weapon 2",
        "Head",
        "Body",
        "Hands",
        "Feet"
    };

    virtual Slot getSlot(void) = 0;
    virtual void setSlot(Slot slot) = 0;
    virtual Item* getItem(void) = 0;
    virtual bool isValid(void) = 0;
    
    virtual T getStats(void) = 0;
    virtual void addTo(Stats::ActorStats& actorStats) = 0;

    static Slot getSlotFromName(const std::string& name) {
        for(int i = 0; i < Slot::COUNT; i++) {
            if(name == SLOT_NAMES[i]) {
                return (Slot) i;
            }
        }

        return Slot::UNDEFINED;
    }
};
