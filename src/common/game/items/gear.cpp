#include "gear.h"

Gear::Gear(Slot slot, const GearStats& stats, Item* item) :
    slot(slot),
    stats(stats),
    item(item)
{ }

Equippable<GearStats>::Slot Gear::getSlot(void) {
    return slot;
}

void Gear::setSlot(Slot slot) {
    this->slot = slot;
}

Item* Gear::getItem(void) {
    return item;
}

bool Gear::isValid(void) {
    return contains(VALID_GEAR_SLOTS, slot);
}

GearStats Gear::getStats(void) {
    return stats;
}

void Gear::addTo(EntityStats& entityStats) {
    entityStats.hp += stats.hp;
    entityStats.armour += stats.armour;
    entityStats.power += stats.power;
    entityStats.speed += stats.speed;
    entityStats.wisdom += stats.wisdom;

    // Temp calculation, needs balancing
    entityStats.movesPerTurn += stats.speed / 5;
}