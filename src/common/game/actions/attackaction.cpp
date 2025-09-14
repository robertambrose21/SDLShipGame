#include "attackaction.h"
#include "game/participant/participant.h"
#include "game/actors/actorpool.h"

AttackAction::AttackAction(
    Participant* participant,
    entt::entity entity,
    entt::entity weaponId,
    const glm::ivec2& target,
    bool isAnimationOnly
) :
    Action(participant, entity),
    weaponId(weaponId),
    target(target),
    isAnimationOnly(isAnimationOnly)
{ }

AttackAction::AttackAction(
    Participant* participant,
    entt::entity entity,
    int turnNumber,
    entt::entity weaponId,
    const glm::ivec2& target,
    bool isAnimationOnly
) : 
    Action(participant, entity, turnNumber),
    weaponId(weaponId),
    target(target),
    isAnimationOnly(isAnimationOnly)
{ }

ActionVariant AttackAction::getPublishData(void) {
    return AttackActionEventData { turnNumber, entity, target, weaponId };
}

Action::Type AttackAction::getType(void) {
    return Action::Type::Attack;
}

entt::entity AttackAction::getWeapon(void) {
    return weaponId;
}

glm::ivec2 AttackAction::getTarget(void) const {
    return target;
}

bool AttackAction::getIsAnimationOnly(void) const {
    return isAnimationOnly;
}

bool AttackAction::passesPrecondition(ApplicationContext* context) {
    auto weaponHolder = context->getEntityRegistry().try_get<WeaponHolder>(weaponId);

    return weaponHolder != nullptr && weaponHolder->weapon != nullptr && weaponHolder->weapon->isInRange(target);
}

bool AttackAction::onValidate(ApplicationContext* context) {
    auto wepaonHolder = context->getEntityRegistry().try_get<WeaponHolder>(weaponId);

    if(wepaonHolder == nullptr || wepaonHolder->weapon == nullptr) {
        spdlog::trace("[Attack]: Failed to validate action, weapon is null");
        return false;
    }

    auto& weapon = wepaonHolder->weapon;
    auto actor = context->getEntityRegistry().try_get<Actor>(entity);

    if(!actor) {
        spdlog::trace("[{}]: Failed to validate action, actor is null", typeToString());
        return false;
    }

    auto const& weaponOwnerPosition = context->getEntityRegistry().get<Position>(entity);

    if(!weapon->isInRange(target)) {
        spdlog::trace(
            "[Attack]: Failed to validate action, weapon is out of range of actor. Weapon[{}#{}] range=({}) pos=({}, {}), targetPos=({}, {})", 
            weapon->getName(),
            weapon->getId().getString(),
            weapon->getStats().range,
            weaponOwnerPosition.x, weaponOwnerPosition.y,
            target.x, target.y
        );
        return false;
    }

    if(weapon->getUsesLeft() == 0 || weapon->getUsesLeft() < numAttacksInChain(context)) {
        spdlog::trace(
            "[Attack]: Failed to validate action, not enough uses Weapon[{}#{}] ({}/{}), chain: {}",
            weapon->getName(),
            weapon->getId().getString(),
            weapon->getUsesLeft(),
            weapon->getStats().uses,
            numAttacksInChain(context)
        );
        return false;
    }

    return true;
}

void AttackAction::onExecute(ApplicationContext* context) {
    auto actor = context->getEntityRegistry().try_get<Actor>(entity);
    auto& weapon = context->getEntityRegistry().get<WeaponHolder>(weaponId).weapon;

    if(!actor) {
        spdlog::trace("[{}]: Failed to execute action, actor is null", typeToString());
        return;
    }

    auto const& position = context->getEntityRegistry().get<Position>(entity);

    // actor->attack(position, target, weapon->getId(), isAnimationOnly);
    weapon->use(position, target, isAnimationOnly);
    context->getActorController()->applyStats(entity);
}

bool AttackAction::hasFinished(ApplicationContext* context) {
    auto& weapon = context->getEntityRegistry().get<WeaponHolder>(weaponId).weapon;

    return !weapon->isAnimationInProgress();
}

int AttackAction::numAttacksInChain(ApplicationContext* context) {
    auto& weapon = context->getEntityRegistry().get<WeaponHolder>(weaponId).weapon;

    if(participant->getEngagement() == nullptr || !turnNumber.has_value()) {
        return weapon->getUsesLeft();
    }

    auto& chain = context->getEntityRegistry().get<ActionChain>(entity).chain;

    if(!chain.contains(turnNumber.value())) {
        return 0;
    }

    int numAttacks = 0;

    for(auto& action : chain.at(turnNumber.value())) {
        if(action->getType() != Action::Type::Attack) {
            continue;
        }

        auto previousWeapon = dynamic_cast<AttackAction*>(action.get())->getWeapon();
        auto previousWeaponName = context->getEntityRegistry().get<WeaponHolder>(previousWeapon).weapon->getName();

        if(weapon->getName() == previousWeaponName) {
            numAttacks++;
        }
    }

    return numAttacks;
}
