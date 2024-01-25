#include "equipment.h"

const std::map<Equipment::Slot, std::string> Equipment::SlotToItemType = {
    { HEAD, "Helm" },
    { BODY, "BodyArmour" },
    { HANDS, "Gloves" },
    { FEET, "Boots" },
};

Equipment::Equipment()
{ }

Equipment::Equipment(Item* item, Slot slot) :
    item(item),
    slot(slot)
{ }

Item* Equipment::getItem(void) {
    return item;
}

Equipment::Slot Equipment::getSlot(void) {
    return slot;
}

void Equipment::setSlot(Slot slot) {
    this->slot == slot;
}

bool Equipment::isValid(Item* item, Slot slot) {
    game_assert(SlotToItemType.size() == Slot::COUNT);

    if(item == nullptr) {
        return false;
    }

    return SlotToItemType.at(slot) == item->getType();
}