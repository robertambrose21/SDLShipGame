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

void Gear::addTo(Stats::ActorStats& actorStats) {
    Stats::addEquipmentStatsToActor(actorStats, getStats());
}