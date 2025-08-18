#include "equipweaponaction.h"

EquipWeaponAction::EquipWeaponAction(
    Participant* participant, 
    Actor* actor, 
    Item* item, 
    const UUID& weaponId,
    bool isUnequip
) :
    Action(participant, actor),
    item(item),
    weaponId(weaponId),
    isUnequip(isUnequip)
{ }

EquipWeaponAction::EquipWeaponAction(
    Participant* participant, 
    Actor* actor,
    int turnNumber,
    Item* item, 
    const UUID& weaponId,
    bool isUnequip
) :
    Action(participant, actor, turnNumber),
    item(item),
    weaponId(weaponId),
    isUnequip(isUnequip)
{ }

ActionVariant EquipWeaponAction::getPublishData(void) {
    return EquipWeaponActionEventData { turnNumber, actor, item, weaponId };
}

bool EquipWeaponAction::onValidate(ApplicationContext* context) {
    if(item == nullptr) {
        spdlog::trace("[EquipWeaponItem]: Failed to validate action, item is null");
        return false;
    }

    if(item->getParticipantId() != actor->getParticipantId()) {
        spdlog::trace(
            "[EquipWeaponItem]: Failed to validate action, item participant ({}) does not match actor participant ({})",
            item->getParticipantId(),
            actor->getParticipantId()
        );
        return false;
    }

    if(actor->getWeapon(weaponId)->getItem()->getId() != item->getId()) {
        spdlog::trace(
            "[EquipWeaponItem]: Failed to validate action, Weapon[{}#{}] item id {} does not match supplied item id {}",
            actor->getWeapon(weaponId)->getName(),
            weaponId.getString(),
            actor->getWeapon(weaponId)->getItem()->getId(),
            item->getId()
        );
        return false;
    }

    if(isUnequip) {
        return validateUnequip(context);
    }

    return validateEquip(context);
}

bool EquipWeaponAction::validateEquip(ApplicationContext* context) {
    auto participant = context->getGameController()->getParticipant(actor->getParticipantId());

    bool hasItem = false;   

    for(auto itemInInventory : participant->getItems()) {
        if(item->getId() == itemInInventory->getId()) {
            hasItem = true;
        }
    }

    if(!hasItem) {
        spdlog::trace(
            "[EquipWeaponAction]: Failed to validate action, cannot find Item[{}] in inventory for participant {}",
            item->getId(),
            participant->getId()
        );
        return false;
    }

    if(actor->hasWeapon(weaponId)) {
        spdlog::trace(
            "[EquipWeaponAction]: Actor already has Weapon[{}#{}] item {}",
            actor->getWeapon(weaponId)->getName(),
            weaponId.getString(),
            item->getId()
        );

        return false;
    }

    return true;
}

bool EquipWeaponAction::validateUnequip(ApplicationContext* context) {
    if(!actor->hasWeapon(weaponId)) {
        spdlog::trace(
            "[EquipWeaponAction(Unequip)]: Actor does not have Weapon[{}#{}] item {}",
            actor->getWeapon(weaponId)->getName(),
            weaponId.getString(),
            item->getId()
        );

        return false;
    }

    return true;
}

void EquipWeaponAction::onExecute(ApplicationContext* context) {
    auto participant = context->getGameController()->getParticipant(actor->getParticipantId());

    if(isUnequip) {
        participant->addItem(item);
        actor->removeWeapon(weaponId);
        return;
    }
    
    if(!actor->hasWeapon(weaponId)) {
        spdlog::trace("Weapon {} doesn't exist, adding", weaponId.getString());
        actor->addWeapon(context->getWeaponController()->createWeapon(weaponId, item->getName(), actor));
    }
    else {
        spdlog::trace("Actor {} already has weapon {}", actor->getId(), weaponId.getString());
    }

    participant->removeItem(item);
}

bool EquipWeaponAction::hasFinished(void) {
    return true;
}

bool EquipWeaponAction::passesPrecondition(void) {
    return !participant->hasAnyEngagement();
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