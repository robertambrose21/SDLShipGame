#include "gear.h"

Gear::Gear(Slot slot, Item* item) :
    slot(slot),
    item(item)
{ }

Equippable<Stats::GearStats>::Slot Gear::getSlot(void) {
    return slot;
}

void Gear::setSlot(Slot slot) {
    this->slot = slot;
}

Item* Gear::getItem(void) {
    return item;
}

bool Gear::isValid(void) {
    return contains(VALID_SLOTS, slot);
}

Stats::GearStats Gear::getStats(void) {
    return item->getStats().gear;
}

void Gear::addTo(Stats::EntityStats& entityStats) {
    entityStats.hp += getStats().hp;
    entityStats.totalHp += getStats().hp;

    entityStats.armour += getStats().armour;
    entityStats.power += getStats().power;
    entityStats.speed += getStats().speed;
    entityStats.wisdom += getStats().wisdom;

    // Temp calculation, needs balancing
    entityStats.movesPerTurn += getStats().speed / 5;
    entityStats.movesLeft += getStats().speed / 5;
}