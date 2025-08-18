#include "attackaction.h"
#include "game/participant/participant.h"

AttackAction::AttackAction(
    Participant* participant,
    Actor* actor,
    Weapon* weapon,
    const glm::ivec2& target,
    bool isAnimationOnly
) :
    Action(participant, actor),
    weapon(weapon),
    target(target),
    isAnimationOnly(isAnimationOnly)
{ }

AttackAction::AttackAction(
    Participant* participant,
    Actor* actor,
    int turnNumber,
    Weapon* weapon,
    const glm::ivec2& target,
    bool isAnimationOnly
) : 
    Action(participant, actor, turnNumber),
    weapon(weapon),
    target(target),
    isAnimationOnly(isAnimationOnly)
{ }

ActionVariant AttackAction::getPublishData(void) {
    return AttackActionEventData { turnNumber, actor, target, weapon };
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

    if(weapon->getUsesLeft() == 0 || weapon->getUsesLeft() < numAttacksInChain()) {
        spdlog::trace(
            "[Attack]: Failed to validate action, not enough uses Weapon[{}#{}] ({}/{}), chain: {}",
            weapon->getName(),
            weapon->getId().getString(),
            weapon->getUsesLeft(),
            weapon->getStats().uses,
            numAttacksInChain()
        );
        return false;
    }

    return true;
}

void AttackAction::onExecute(ApplicationContext* context) {
    actor->attack(target, weapon->getId(), isAnimationOnly);
}

bool AttackAction::hasFinished(void) {
    return !weapon->isAnimationInProgress();
}

int AttackAction::numAttacksInChain(void) {
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
