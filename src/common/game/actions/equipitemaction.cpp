#include "equipitemaction.h"

EquipItemAction::EquipItemAction(int turnNumber, Entity* entity, Item* item, Equipment::Slot slot, bool isUnequip) :
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

    if(!Equipment::isValid(item, slot)) {
        return false;
    }

    if(isUnequip) {
        return entity->getEquipment(slot) != nullptr;
    }

    auto participant = context->getTurnController()->getParticipant(entity->getParticipantId());

    bool hasItem = false;   

    for(auto itemInInventory : participant->items) {
        if(item->getId() == itemInInventory->getId()) {
            hasItem = true;
        }
    }

    return hasItem;
}

void EquipItemAction::onExecute(ApplicationContext* context) {
    auto participant = context->getTurnController()->getParticipant(entity->getParticipantId());
    auto existingEquipment = entity->getEquipment(slot);

    if(isUnequip) {
        participant->items.push_back(existingEquipment->getItem());
        entity->removeEquipment(slot);
    } else {
        if(existingEquipment != nullptr) {
            participant->items.push_back(existingEquipment->getItem());
        }

        std::erase(participant->items, item);
        entity->setEquipment(item, slot);
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

Equipment::Slot EquipItemAction::getSlot(void) const {
    return slot;
}

bool EquipItemAction::getIsUnequip(void) const {
    return isUnequip;
}