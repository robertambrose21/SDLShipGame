#include "moveaction.h"
#include "game/participant/participant.h"

MoveAction::MoveAction(
    Participant* participant,
    Actor* actor, 
    const glm::ivec2& position,
    int shortStopSteps
) : 
    Action(participant, actor),
    position(position),
    shortStopSteps(shortStopSteps)
{
    game_assert(shortStopSteps >= 0);
}

MoveAction::MoveAction(
    Participant* participant,
    Actor* actor,
    int turnNumber,
    const glm::ivec2& position,
    int shortStopSteps
) : 
    Action(participant, actor, turnNumber),
    position(position),
    shortStopSteps(shortStopSteps)
{
    game_assert(shortStopSteps >= 0);
}

ActionVariant MoveAction::getPublishData(void) {
    return MoveActionEventData { turnNumber, actor, position, shortStopSteps };
}

Action::Type MoveAction::getType(void) {
    return Action::Type::Move;
}

bool MoveAction::passesPrecondition(void) {
    return true;
}

bool MoveAction::onValidate(ApplicationContext* context) {
    if(!actor->isEngaged()) {
        auto hasPath = !getPath().empty();

        if(!hasPath) {
            spdlog::trace(
                "[Move]: Failed to validate action, Actor[{}#{}] not engaged but has no path",
                actor->getName(),
                actor->getId()
            );
        }

        return hasPath;
    }

    if(actor->getMovesLeft() <= 0) {
        spdlog::trace(
            "[Move]: Failed to validate action, Actor[{}#{}] has (0/{}) moves left",
            actor->getName(),
            actor->getId(),
            actor->getStats().movesPerTurn
        );
        return false;
    }

    if(getPath().empty()) {
        spdlog::trace(
            "[Move]: Failed to validate action, Actor[{}#{}] has no path",
            actor->getName(),
            actor->getId()
        );
        return false;
    }


    if(!hasAvailableMoves()) {
        spdlog::trace(
            "[Move]: Failed to validate action, Actor[{}#{}] has ({}/{}) moves left but not enough left in chain",
            actor->getName(),
            actor->getId(),
            actor->getMovesLeft(),
            actor->getStats().movesPerTurn
        );
        return false;
    }

    return true;
}

void MoveAction::onExecute(ApplicationContext* context) {
    actor->setPath(getPath(true));
}

bool MoveAction::hasFinished(void) {
    return actor->getMovesLeft() <= 0 || actor->getPosition() == path[path.size() - shortStopSteps - 1];
}

std::deque<glm::ivec2> MoveAction::getPath(bool recalculate) {
    if(recalculate || path.empty()) {
        path = actor->calculatePath(position, shortStopSteps);
    }

    return path;
}

bool MoveAction::hasAvailableMoves(void) {
    if(participant->getEngagement() == nullptr || !turnNumber.has_value()) {
        return true;
    }

    int numMoves = 0;
 
    for(auto& action : actor->getActionsChain(turnNumber.value())) {
        if(action->getType() == Action::Type::Move) {
            numMoves += dynamic_cast<MoveAction*>(action)->getPath().size();
        }
    }

    return actor->getMovesLeft() >= numMoves;
}

glm::ivec2 MoveAction::getPosition(void) const {
    return position;
}

int MoveAction::getShortStopSteps(void) const {
    return shortStopSteps;
}