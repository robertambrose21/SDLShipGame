#include "attackaction.h"

AttackAction::AttackAction(
    int turnNumber,
    Entity* entity,
    Weapon* weapon,
    const glm::ivec2& target,
    bool isAnimationOnly
) :
    Action(turnNumber, entity),
    weapon(weapon),
    target(target),
    isAnimationOnly(isAnimationOnly)
{ }

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
        return false;
    }

    if(!weapon->isInRange(target)) {
        return false;
    }

    if(weapon->getUsesLeft() == 0 || weapon->getUsesLeft() < numAttacksInChain()) {
        return false;
    }

    return true;
}

void AttackAction::onExecute(ApplicationContext* context) {
    entity->attack(target, weapon->getId(), isAnimationOnly);
}

bool AttackAction::hasFinished(void) {
    return !weapon->isAnimationInProgress();
}

int AttackAction::numAttacksInChain(void) {
    int numAttacks = 0;

    for(auto& action : entity->getActionsChain(turnNumber)) {
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
