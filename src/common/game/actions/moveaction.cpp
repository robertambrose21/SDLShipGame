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
        auto hasPath = !getPath().empty();

        if(!hasPath) {
            spdlog::trace(
                "[{}, Move]: Failed to validate action, Entity[{}#{}] not engaged but has no path",
                turnNumber,
                entity->getName(),
                entity->getId()
            );
        }

        return hasPath;
    }

    if(entity->getMovesLeft() <= 0) {
        spdlog::trace(
            "[{}, Move]: Failed to validate action, Entity[{}#{}] has (0/{}) moves left",
            turnNumber,
            entity->getName(),
            entity->getId(),
            entity->getStats().movesPerTurn
        );
        return false;
    }

    if(getPath().empty()) {
        spdlog::trace(
            "[{}, Move]: Failed to validate action, Entity[{}#{}] has no path",
            turnNumber,
            entity->getName(),
            entity->getId()
        );
        return false;
    }


    if(!hasAvailableMoves()) {
        spdlog::trace(
            "[{}, Move]: Failed to validate action, Entity[{}#{}] has ({}/{}) moves left but not enough left in chain",
            turnNumber,
            entity->getName(),
            entity->getId(),
            entity->getMovesLeft(),
            entity->getStats().movesPerTurn
        );
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