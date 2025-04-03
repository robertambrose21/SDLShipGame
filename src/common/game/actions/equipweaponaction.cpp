#include "equipweaponaction.h"

EquipWeaponAction::EquipWeaponAction(
    Participant* participant, 
    Entity* entity, 
    Item* item, 
    const UUID& weaponId,
    bool isUnequip
) :
    Action(participant, entity),
    item(item),
    weaponId(weaponId),
    isUnequip(isUnequip)
{ }

EquipWeaponAction::EquipWeaponAction(
    Participant* participant, 
    Entity* entity,
    int turnNumber,
    Item* item, 
    const UUID& weaponId,
    bool isUnequip
) :
    Action(participant, entity, turnNumber),
    item(item),
    weaponId(weaponId),
    isUnequip(isUnequip)
{ }

void EquipWeaponAction::publish(ActionPublisher& publisher) {
    publisher.publish<EquipWeaponActionEventData>({ turnNumber, entity, item, weaponId });
}

bool EquipWeaponAction::onValidate(ApplicationContext* context) {
    if(item == nullptr) {
        spdlog::trace("[EquipWeaponItem]: Failed to validate action, item is null");
        return false;
    }

    if(item->getParticipantId() != entity->getParticipantId()) {
        spdlog::trace(
            "[EquipWeaponItem]: Failed to validate action, item participant ({}) does not match entity participant ({})",
            item->getParticipantId(),
            entity->getParticipantId()
        );
        return false;
    }

    if(entity->getWeapon(weaponId)->getItem()->getId() != item->getId()) {
        spdlog::trace(
            "[EquipWeaponItem]: Failed to validate action, Weapon[{}#{}] item id {} does not match supplied item id {}",
            entity->getWeapon(weaponId)->getName(),
            weaponId.getString(),
            entity->getWeapon(weaponId)->getItem()->getId(),
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
    auto participant = context->getGameController()->getParticipant(entity->getParticipantId());

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

    if(entity->hasWeapon(weaponId)) {
        spdlog::trace(
            "[EquipWeaponAction]: Entity already has Weapon[{}#{}] item {}",
            entity->getWeapon(weaponId)->getName(),
            weaponId.getString(),
            item->getId()
        );

        return false;
    }

    return true;
}

bool EquipWeaponAction::validateUnequip(ApplicationContext* context) {
    if(!entity->hasWeapon(weaponId)) {
        spdlog::trace(
            "[EquipWeaponAction(Unequip)]: Entity does not have Weapon[{}#{}] item {}",
            entity->getWeapon(weaponId)->getName(),
            weaponId.getString(),
            item->getId()
        );

        return false;
    }

    return true;
}

void EquipWeaponAction::onExecute(ApplicationContext* context) {
    auto participant = context->getGameController()->getParticipant(entity->getParticipantId());

    if(isUnequip) {
        participant->addItem(item);
        entity->removeWeapon(weaponId);
        return;
    }
    
    if(!entity->hasWeapon(weaponId)) {
        spdlog::trace("Weapon {} doesn't exist, adding", weaponId.getString());
        entity->addWeapon(context->getWeaponController()->createWeapon(weaponId, item->getName(), entity));
    }
    else {
        spdlog::trace("Entity {} already has weapon {}", entity->getId(), weaponId.getString());
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