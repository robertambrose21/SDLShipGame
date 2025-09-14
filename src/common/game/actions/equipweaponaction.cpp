#include "equipweaponaction.h"

EquipWeaponAction::EquipWeaponAction(
    Participant* participant, 
    entt::entity entity, 
    Item* item, 
    const UUID& weaponUUID,
    bool isUnequip
) :
    Action(participant, entity),
    item(item),
    weaponUUID(weaponUUID),
    weaponId(entt::null),
    isUnequip(isUnequip)
{ }

EquipWeaponAction::EquipWeaponAction(
    Participant* participant, 
    entt::entity entity,
    int turnNumber,
    Item* item, 
    const UUID& weaponUUID,
    bool isUnequip
) :
    Action(participant, entity, turnNumber),
    item(item),
    weaponUUID(weaponUUID),
    weaponId(entt::null),
    isUnequip(isUnequip)
{ }

ActionVariant EquipWeaponAction::getPublishData(void) {
    return EquipWeaponActionEventData { turnNumber, entity, item, weaponId };
}

bool EquipWeaponAction::onValidate(ApplicationContext* context) {
    if(item == nullptr) {
        spdlog::trace("[EquipWeaponItem]: Failed to validate action, item is null");
        return false;
    }

    auto actor = context->getEntityRegistry().try_get<Actor>(entity);

    if(!actor) {
        spdlog::trace("[{}]: Failed to validate action, actor is null", typeToString());
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

    // if(!actor->hasWeapon(weaponId)) {
    //     spdlog::trace(
    //         "[EquipWeaponItem]: Failed to validate action, actor does not have weapon entity {}",
    //         static_cast<entt::id_type>(weaponId)
    //     );
    //     return false;
    // }

    // auto& weapon = context->getEntityRegistry().get<WeaponHolder>(weaponId);

    // if(weapon.getItem()->getId() != item->getId()) {
    //     spdlog::trace(
    //         "[EquipWeaponItem]: Failed to validate action, Weapon[{}#{}] item id {} does not match supplied item id {}",
    //         weapon.getName(),
    //         weapon.getId().getString(),
    //         weapon.getItem()->getId(),
    //         item->getId()
    //     );
    //     return false;
    // }

    if(isUnequip) {
        return validateUnequip(context, actor);
    }

    return validateEquip(context, actor);
}

bool EquipWeaponAction::validateEquip(ApplicationContext* context, Actor* actor) {
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
        auto& weapon = context->getEntityRegistry().get<WeaponHolder>(weaponId).weapon;

        spdlog::trace(
            "[EquipWeaponAction]: Actor already has Weapon[{}#{}] item {}",
            weapon->getName(),
            weapon->getId().getString(),
            item->getId()
        );

        return false;
    }

    return true;
}

bool EquipWeaponAction::validateUnequip(ApplicationContext* context, Actor* actor) {
    if(!actor->hasWeapon(weaponId)) {
        auto& weapon = context->getEntityRegistry().get<WeaponHolder>(weaponId).weapon;

        spdlog::trace(
            "[EquipWeaponAction(Unequip)]: Actor does not have Weapon[{}#{}] item {}",
            weapon->getName(),
            weapon->getId().getString(),
            item->getId()
        );

        return false;
    }

    return true;
}

void EquipWeaponAction::onExecute(ApplicationContext* context) {
    auto actor = context->getEntityRegistry().try_get<Actor>(entity);

    if(!actor) {
        spdlog::trace("[{}]: Failed to execute action, actor is null", typeToString());
        return;
    }

    auto participant = context->getGameController()->getParticipant(actor->getParticipantId());

    // TODO: Move to UnequipWeaponAction
    if(isUnequip) {
        participant->addItem(item);
        actor->removeWeapon(weaponId);
        return;
    }

    // auto weapon = context->getEntityRegistry().try_get<WeaponHolder>(weaponId);

    // if(weapon == nullptr) {
    //     spdlog::trace(
    //         "[EquipWeaponAction]: Failed to execute action, cannot find Weapon entity {} in registry",
    //         static_cast<entt::id_type>(weaponId)
    //     );
    //     return;
    // }
    
    // if(!actor->hasWeapon(weaponId)) {
    //     spdlog::trace("Weapon {} doesn't exist, adding", weapon->getId().getString());
    //     // context->getWeaponController()->addWeapon()
    //     // actor->addWeapon(context->getWeaponController()->createWeapon(weaponId, item->getName(), entity));
    // }
    // else {
    //     spdlog::trace("Actor {} already has weapon {}", actor->getId(), weapon->getId().getString());
    // }

    auto existingWeaponId = context->getWeaponController()->getByExternalId(weaponUUID);

    if(existingWeaponId.has_value()) {
        spdlog::trace(
            "[EquipWeaponAction]: Failed to execute action, weapon with UUID {} already exists as entity {}",
            weaponUUID.getString(),
            static_cast<entt::id_type>(existingWeaponId.value())
        );
        return;
    }

    weaponId = context->getWeaponController()->addWeapon(weaponUUID, item->getName(), entity);

    context->getActorController()->applyStats(entity);
    participant->removeItem(item);
}

bool EquipWeaponAction::hasFinished(ApplicationContext* context) {
    return true;
}

bool EquipWeaponAction::passesPrecondition(ApplicationContext* context) {
    return !participant->hasAnyEngagement();
}

Action::Type EquipWeaponAction::getType(void) {
    return EquipWeaponItem;
}

Item* EquipWeaponAction::getItem(void) {
    return item;
}

UUID EquipWeaponAction::getWeaponUUID(void) const {
    return weaponUUID;
}