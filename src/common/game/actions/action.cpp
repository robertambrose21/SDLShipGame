#include "action.h"

Action::Action(Participant* participant, entt::entity entity) :
    participant(participant),
    entity(entity),
    _isExecuted(false), 
    turnNumber(participant->getEngagement() != nullptr ? 
        std::optional<int>(participant->getEngagement()->getTurnNumber()) : 
        std::nullopt
    )
{ }

Action::Action(Participant* participant, entt::entity entity, int turnNumber) :
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
    if(!passesPrecondition(context)) {
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

bool Action::isFinished(ApplicationContext* context) {
    if(!isExecuted()) {
        return false;
    }

    return hasFinished(context);
}

Participant* Action::getParticipant(void) {
    return participant;
}

entt::entity Action::getEntity(void) const {
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
        case UnequipWeaponItem: return "UnequipWeaponItem";
        default: return "Unknown type: " + getType();
    }
}