#include "equipitemaction.h"

EquipItemAction::EquipItemAction(
    int turnNumber, 
    Entity* entity, 
    Item* item, 
    Equippable<GearStats>::Slot slot, 
    bool isUnequip
) :
    Action(turnNumber, entity),
    item(item),
    slot(slot),
    isUnequip(isUnequip)
{ }

bool EquipItemAction::onValidate(ApplicationContext* context) {
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

void EquipItemAction::onExecute(ApplicationContext* context) {
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

bool EquipItemAction::hasFinished(void) {
    return true;
}

bool EquipItemAction::passesPrecondition(void) {
    return true;
}

Action::Type EquipItemAction::getType(void) {
    return Action::Type::EquipItem;
}

Item* EquipItemAction::getItem(void) {
    return item;
}

Equippable<GearStats>::Slot EquipItemAction::getSlot(void) const {
    return slot;
}

bool EquipItemAction::getIsUnequip(void) const {
    return isUnequip;
}