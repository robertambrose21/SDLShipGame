#include "equipgearaction.h"

EquipGearAction::EquipGearAction(
    int turnNumber, 
    Entity* entity, 
    Item* item, 
    Equippable<Stats::GearStats>::Slot slot, 
    bool isUnequip
) :
    Action(turnNumber, entity),
    item(item),
    slot(slot),
    isUnequip(isUnequip)
{ }

bool EquipGearAction::onValidate(ApplicationContext* context) {
    if(item == nullptr) {
        return false;
    }

    if(item->getParticipantId() != entity->getParticipantId()) {
        return false;
    }

    if(!contains(Gear::VALID_SLOTS, slot)) {
        return false;
    }

    if(isUnequip) {
        return entity->getGear(slot) != nullptr;
    }

    auto participant = context->getTurnController()->getParticipant(entity->getParticipantId());

    bool hasItem = false;   

    for(auto itemInInventory : participant->getItems()) {
        if(item->getId() == itemInInventory->getId()) {
            hasItem = true;
        }
    }

    return hasItem;
}

void EquipGearAction::onExecute(ApplicationContext* context) {
    auto participant = context->getTurnController()->getParticipant(entity->getParticipantId());
    auto existingGear = entity->getGear(slot);

    if(isUnequip) {
        participant->addItem(existingGear->getItem());
        entity->removeGear(slot);
    } else {
        if(existingGear != nullptr) {
            participant->addItem(existingGear->getItem());
        }

        participant->removeItem(item);
        entity->setGear(std::make_unique<Gear>(slot, item));
    }
}

bool EquipGearAction::hasFinished(void) {
    return true;
}

bool EquipGearAction::passesPrecondition(void) {
    return true;
}

Action::Type EquipGearAction::getType(void) {
    return Action::Type::EquipItem;
}

Item* EquipGearAction::getItem(void) {
    return item;
}

Equippable<Stats::GearStats>::Slot EquipGearAction::getSlot(void) const {
    return slot;
}

bool EquipGearAction::getIsUnequip(void) const {
    return isUnequip;
}