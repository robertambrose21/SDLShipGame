#include "action.h"

Action::Action(Participant* participant, Actor* actor) :
    participant(participant),
    actor(actor),
    _isExecuted(false), 
    turnNumber(participant->getEngagement() != nullptr ? 
        std::optional<int>(participant->getEngagement()->getTurnNumber()) : 
        std::nullopt
    )
{ }

Action::Action(Participant* participant, Actor* actor, int turnNumber) :
    participant(participant),
    actor(actor),
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
    if(actor == nullptr) {
        spdlog::trace("[{}]: Failed to validate action, actor is null", typeToString());
        return false;
    }

    if(actor->getCurrentHP() <= 0) {
        spdlog::trace(
            "[{}]: Failed to validate action, Actor[{}#{}] hp is {}", 
            typeToString(),
            actor->getName(),
            actor->getId(), 
            actor->getCurrentHP()
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

Actor* Action::getActor(void) {
    return actor;
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