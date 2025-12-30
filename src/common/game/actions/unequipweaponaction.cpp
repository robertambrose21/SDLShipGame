#include "unequipweaponaction.h"

UnequipWeaponAction::UnequipWeaponAction(
    Participant* participant, 
    entt::entity entity, 
    Item* item, 
    entt::entity weaponId
) :
    Action(participant, entity),
    item(item),
    weaponId(weaponId)
{ }

UnequipWeaponAction::UnequipWeaponAction(
    Participant* participant, 
    entt::entity entity, 
    int turnNumber, 
    Item* item, 
    entt::entity weaponId
) :
    Action(participant, entity, turnNumber),
    item(item),
    weaponId(weaponId)
{ }

ActionVariant UnequipWeaponAction::getPublishData(void) {
    return UnequipWeaponActionEventData { turnNumber, entity, item, weaponId };
}

bool UnequipWeaponAction::passesPrecondition(ApplicationContext* context) {
    return !participant->hasAnyEngagement();
}

Action::Type UnequipWeaponAction::getType(void) {
    return UnequipWeaponItem;
}

Item* UnequipWeaponAction::getItem(void) {
    return item;
}

entt::entity UnequipWeaponAction::getWeaponId(void) const {
    return weaponId;
}

bool UnequipWeaponAction::onValidate(ApplicationContext* context) {
    if(item == nullptr) {
        spdlog::trace("[UnequipWeaponItem]: Failed to validate action, item is null");
        return false;
    }

    auto actor = context->getEntityRegistry().try_get<Actor>(entity);

    if(!actor) {
        spdlog::trace("[{}]: Failed to validate action, actor is null", typeToString());
        return false;
    }

    if(item->getParticipantId() != actor->getParticipantId()) {
        spdlog::trace(
            "[UnequipWeaponItem]: Failed to validate action, item participant ({}) does not match actor participant ({})",
            item->getParticipantId(),
            actor->getParticipantId()
        );
        return false;
    }

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

void UnequipWeaponAction::onExecute(ApplicationContext* context) {
    auto actor = context->getEntityRegistry().try_get<Actor>(entity);

    if(!actor) {
        spdlog::trace("[{}]: Failed to execute action, actor is null", typeToString());
        return;
    }

    auto participant = context->getGameController()->getParticipant(actor->getParticipantId());

    participant->addItem(item);
    actor->removeWeapon(weaponId);
    context->getActorController()->applyStats(entity);
    return;
}

bool UnequipWeaponAction::hasFinished(ApplicationContext* context) {
    return true;
}