#include "action.h"
#include "game/participant/participant.h"

Action::Action(Participant* participant, Entity* entity) :
    participant(participant),
    entity(entity),
    _isExecuted(false), 
    turnNumber(participant->getEngagement() != nullptr ? 
        std::optional<int>(participant->getEngagement()->getTurnNumber()) : 
        std::nullopt
    )
{ }

Action::Action(Participant* participant, Entity* entity, int turnNumber) :
    participant(participant),
    entity(entity),
    _isExecuted(false),
    turnNumber(std::optional<int>(turnNumber))
{
    if(participant->getEngagement() == nullptr) {
        spdlog::warn(
            "[{}]: Turn number {} is set, but supplied participant {} is not in an engagement",
            typeToString(),
            turnNumber,
            participant->getId()
        );
    }
}

bool Action::validate(ApplicationContext* context) {
    if(entity == nullptr) {
        spdlog::trace("[{}]: Failed to validate action, entity is null", typeToString());
        return false;
    }

    if(entity->getCurrentHP() <= 0) {
        spdlog::trace(
            "[{}]: Failed to validate action, Entity[{}#{}] hp is {}", 
            typeToString(),
            entity->getName(),
            entity->getId(), 
            entity->getCurrentHP()
        );
        return false;
    }

    if(!passesPrecondition()) {
        spdlog::trace("[{}]: Failed to validate action, failed precondition", typeToString());
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

Participant* Action::getParticipant(void) {
    return participant;
}

Entity* Action::getEntity(void) {
    return entity;
}

bool Action::isExecuted(void) const {
    return _isExecuted;
}

std::optional<int> Action::getTurnNumber(void) const {
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