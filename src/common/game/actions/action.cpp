#include "action.h"

Action::Action(Entity* entity) :
    entity(entity),
    _isExecuted(false)
{ }

bool Action::validate(void) {
    if(entity == nullptr) {
        return false;
    }

    if(entity->getCurrentHP() <= 0) {
        return false;
    }

    return onValidate();
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