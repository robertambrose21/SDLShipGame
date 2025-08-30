#include "attackaction.h"
#include "game/participant/participant.h"

AttackAction::AttackAction(
    Participant* participant,
    entt::entity entity,
    Weapon* weapon,
    const glm::ivec2& target,
    bool isAnimationOnly
) :
    Action(participant, entity),
    weapon(weapon),
    target(target),
    isAnimationOnly(isAnimationOnly)
{ }

AttackAction::AttackAction(
    Participant* participant,
    entt::entity entity,
    int turnNumber,
    Weapon* weapon,
    const glm::ivec2& target,
    bool isAnimationOnly
) : 
    Action(participant, entity, turnNumber),
    weapon(weapon),
    target(target),
    isAnimationOnly(isAnimationOnly)
{ }

ActionVariant AttackAction::getPublishData(void) {
    return AttackActionEventData { turnNumber, entity, target, weapon };
}

Action::Type AttackAction::getType(void) {
    return Action::Type::Attack;
}

Weapon* AttackAction::getWeapon(void) {
    return weapon;
}

glm::ivec2 AttackAction::getTarget(void) const {
    return target;
}

bool AttackAction::getIsAnimationOnly(void) const {
    return isAnimationOnly;
}

bool AttackAction::passesPrecondition(void) {
    return weapon != nullptr && weapon->isInRange(target);
}

bool AttackAction::onValidate(ApplicationContext* context) {
    if(weapon == nullptr) {
        spdlog::trace("[Attack]: Failed to validate action, weapon is null");
        return false;
    }

    if(!weapon->isInRange(target)) {
        spdlog::trace(
            "[Attack]: Failed to validate action, weapon is out of range of actor. Weapon[{}#{}] range=({}) pos=({}, {}), targetPos=({}, {})", 
            weapon->getName(),
            weapon->getId().getString(),
            weapon->getStats().range,
            weapon->getOwner()->getPosition().x, weapon->getOwner()->getPosition().y,
            target.x, target.y
        );
        return false;
    }

    auto actor = context->getEntityRegistry().try_get<Actor>(entity);

    if(!actor) {
        spdlog::trace("[{}]: Failed to validate action, actor is null", typeToString());
        return false;
    }

    if(weapon->getUsesLeft() == 0 || weapon->getUsesLeft() < numAttacksInChain(actor)) {
        spdlog::trace(
            "[Attack]: Failed to validate action, not enough uses Weapon[{}#{}] ({}/{}), chain: {}",
            weapon->getName(),
            weapon->getId().getString(),
            weapon->getUsesLeft(),
            weapon->getStats().uses,
            numAttacksInChain(actor)
        );
        return false;
    }

    return true;
}

void AttackAction::onExecute(ApplicationContext* context) {
    auto actor = context->getEntityRegistry().try_get<Actor>(entity);

    if(!actor) {
        spdlog::trace("[{}]: Failed to execute action, actor is null", typeToString());
        return;
    }

    actor->attack(actor->getPosition(), target, weapon->getId(), isAnimationOnly);
}

bool AttackAction::hasFinished(ApplicationContext* context) {
    return !weapon->isAnimationInProgress();
}

int AttackAction::numAttacksInChain(Actor* actor) {
    if(participant->getEngagement() == nullptr || !turnNumber.has_value()) {
        return weapon->getUsesLeft();
    }

    int numAttacks = 0;

    for(auto& action : actor->getActionsChain(turnNumber.value())) {
        if(action->getType() != Action::Type::Attack) {
            continue;
        }

        auto previousWeapon = dynamic_cast<AttackAction*>(action)->getWeapon();

        if(weapon->getName() == previousWeapon->getName()) {
            numAttacks++;
        }
    }

    return numAttacks;
}
