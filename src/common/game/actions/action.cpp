#include "action.h"

Action::Action(int turnNumber, Entity* entity) :
    turnNumber(turnNumber),
    entity(entity),
    _isExecuted(false)
{ }

bool Action::validate(ApplicationContext* context) {
    if(entity == nullptr) {
        return false;
    }

    if(entity->getCurrentHP() <= 0) {
        return false;
    }

    if(!passesPrecondition()) {
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