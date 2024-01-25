#include "moveaction.h"

MoveAction::MoveAction(
    int turnNumber,
    Entity* entity, 
    const glm::ivec2& position,
    int shortStopSteps
) : 
    Action(turnNumber, entity),
    position(position),
    shortStopSteps(shortStopSteps)
{
    game_assert(shortStopSteps >= 0);
}

Action::Type MoveAction::getType(void) {
    return Action::Type::Move;
}

bool MoveAction::passesPrecondition(void) {
    return true;
}

bool MoveAction::onValidate(ApplicationContext* context) {
    if(!entity->isEngaged()) {
        return !getPath().empty();
    }

    if(entity->getMovesLeft() <= 0) {
        return false;
    }

    if(getPath().empty()) {
        return false;
    }


    if(!hasAvailableMoves()) {
        return false;
    }

    return true;
}

void MoveAction::onExecute(ApplicationContext* context) {
    entity->setPath(getPath(true));
}

bool MoveAction::hasFinished(void) {
    return entity->getMovesLeft() <= 0 || entity->getPosition() == path[path.size() - shortStopSteps - 1];
}

std::deque<glm::ivec2> MoveAction::getPath(bool recalculate) {
    if(recalculate || path.empty()) {
        path = entity->calculatePath(position, shortStopSteps);
    }

    return path;
}

bool MoveAction::hasAvailableMoves(void) {
    int numMoves = 0;

    for(auto& action : entity->getActionsChain(turnNumber)) {
        if(action->getType() == Action::Type::Move) {
            numMoves += dynamic_cast<MoveAction*>(action)->getPath().size();
        }
    }

    return entity->getMovesLeft() >= numMoves;
}

glm::ivec2 MoveAction::getPosition(void) const {
    return position;
}

int MoveAction::getShortStopSteps(void) const {
    return shortStopSteps;
}