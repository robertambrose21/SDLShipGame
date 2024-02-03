#include "equipweaponaction.h"

EquipWeaponAction::EquipWeaponAction(int turnNumber, Entity* entity, Item* item, Weapon* toUnequip) :
    Action(turnNumber, entity),
    item(item),
    toUnequip(toUnequip)
{ }

bool EquipWeaponAction::onValidate(ApplicationContext* context) {
    if(item == nullptr) {
        return false;
    }

    if(item->getParticipantId() != entity->getParticipantId()) {
        return false;
    }

    if(toUnequip != nullptr) {
        // TOOD: use weapons map
        for(auto weapon : entity->getWeapons()) {
            if(weapon->getId() == toUnequip->getId()) {
                return true;
            }
        }

        return false;
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

void EquipWeaponAction::onExecute(ApplicationContext* context) {
    auto participant = context->getTurnController()->getParticipant(entity->getParticipantId());

    if(toUnequip != nullptr) {
        participant->items.push_back(item);
        entity->removeWeapon(toUnequip->getId());
    }
    else {
        entity->addWeapon(context->getWeaponController()->createWeapon(item->getName(), entity));
        std::erase(participant->items, item);
    }
}

bool EquipWeaponAction::hasFinished(void) {
    return true;
}

bool EquipWeaponAction::passesPrecondition(void) {
    return true;
}

Action::Type EquipWeaponAction::getType(void) {
    return EquipWeaponItem;
}

Item* EquipWeaponAction::getItem(void) {
    return item;
}

Weapon* EquipWeaponAction::getToUnequip(void) {
    return toUnequip;
}