#include "equipweaponaction.h"

EquipWeaponAction::EquipWeaponAction(int turnNumber, Entity* entity, Item* item, const UUID& weaponId) :
    Action(turnNumber, entity),
    item(item),
    weaponId(weaponId)
{ }

bool EquipWeaponAction::onValidate(ApplicationContext* context) {
    if(item == nullptr) {
        spdlog::trace("[{}, EquipWeaponItem]: Failed to validate action, item is null", turnNumber);
        return false;
    }

    if(item->getParticipantId() != entity->getParticipantId()) {
        spdlog::trace(
            "[{}, EquipWeaponItem]: Failed to validate action, item participant ({}) does not match entity participant ({})",
            turnNumber,
            item->getParticipantId(),
            entity->getParticipantId()
        );
        return false;
    }

    if(entity->hasWeapon(weaponId)) {
        auto alreadyHasWeapon =  entity->getWeapon(weaponId)->getItem()->getId() == item->getId();

        if(alreadyHasWeapon) {
            spdlog::trace(
                "[{}, EquipWeaponItem]: Entity already has Weapon[{}#{}] item {}", 
                turnNumber,
                entity->getWeapon(weaponId)->getName(),
                weaponId.getString(),
                item->getId()
            );
        }
        else {
            spdlog::trace(
                "[{}, EquipWeaponItem]: Failed to validate action, Weapon[{}#{}] item id {} does not match supplied item id {}", 
                turnNumber,
                entity->getWeapon(weaponId)->getName(),
                weaponId.getString(),
                entity->getWeapon(weaponId)->getItem()->getId(),
                item->getId()
            );
        }

        return alreadyHasWeapon;
    }

    auto participant = context->getTurnController()->getParticipant(entity->getParticipantId());

    bool hasItem = false;   

    for(auto itemInInventory : participant->getItems()) {
        if(item->getId() == itemInInventory->getId()) {
            hasItem = true;
        }
    }

    if(!hasItem) {
        spdlog::trace(
            "[{}, EquipWeaponItem]: Failed to validate action, cannot find Item[{}] in inventory for participant {}",
            turnNumber,
            item->getId(),
            participant->getId()
        );
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