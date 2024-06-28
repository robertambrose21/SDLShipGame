#include "equipweaponaction.h"

EquipWeaponAction::EquipWeaponAction(int turnNumber, Entity* entity, Item* item, const UUID& weaponId) :
    Action(turnNumber, entity),
    item(item),
    weaponId(weaponId)
{ }

bool EquipWeaponAction::onValidate(ApplicationContext* context) {
    if(item == nullptr) {
        return false;
    }

    if(item->getParticipantId() != entity->getParticipantId()) {
        return false;
    }

    if(entity->hasWeapon(weaponId)) {
        return entity->getWeapon(weaponId)->getItem()->getId() == item->getId();
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

void EquipWeaponAction::onExecute(ApplicationContext* context) {
    auto participant = context->getTurnController()->getParticipant(entity->getParticipantId());

    if(entity->hasWeapon(weaponId)) {
        participant->addItem(item);
        entity->removeWeapon(weaponId);
    }
    else {
        entity->addWeapon(context->getWeaponController()->createWeapon(weaponId, item->getName(), entity));
        participant->removeItem(item);
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

UUID EquipWeaponAction::getWeaponId(void) const {
    return weaponId;
}