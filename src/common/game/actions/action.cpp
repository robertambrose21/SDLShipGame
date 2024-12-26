#include "action.h"

Action::Action(int turnNumber, Entity* entity) :
    turnNumber(turnNumber),
    entity(entity),
    _isExecuted(false)
{ }

bool Action::validate(ApplicationContext* context) {
    if(entity == nullptr) {
        spdlog::trace("[{}, {}]: Failed to validate action, entity is null", turnNumber, typeToString());
        return false;
    }

    if(entity->getCurrentHP() <= 0) {
        spdlog::trace(
            "[{}, {}]: Failed to validate action, Entity[{}#{}] hp is {}", 
            turnNumber,
            typeToString(),
            entity->getName(),
            entity->getId(), 
            entity->getCurrentHP()
        );
        return false;
    }

    if(!passesPrecondition()) {
        spdlog::trace("[{}, {}]: Failed to validate action, failed precondition", turnNumber, typeToString());
        return false;
    }

    return onValidate(context);
}

void Action::execute(ApplicationContext* context) {
    if(isExecuted()) {
        return;
    }

    onExecute(context);
    _isExecuted = true;
}

bool Action::isFinished(void) {
    if(!isExecuted()) {
        return false;
    }

    return hasFinished();
}

Entity* Action::getEntity(void) {
    return entity;
}

bool Action::isExecuted(void) const {
    return _isExecuted;
}

int Action::getTurnNumber(void) const {
    return turnNumber;
}

std::string Action::typeToString(void) {
    switch (getType()) {
        case Move: return "Move";
        case Attack: return "Attack";
        case TakeItem: return "TakeItem";
        case EquipItem: return "EquipItem";
        case EquipWeaponItem: return "EquipWeaponItem";
        default: return "Unknown type: " + getType();
    }
}