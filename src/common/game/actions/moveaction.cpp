#include "moveaction.h"
#include "game/participant/participant.h"

MoveAction::MoveAction(
    Participant* participant,
    Entity* entity, 
    const glm::ivec2& position,
    int shortStopSteps
) : 
    Action(participant, entity),
    position(position),
    shortStopSteps(shortStopSteps)
{
    game_assert(shortStopSteps >= 0);
}

MoveAction::MoveAction(
    Participant* participant,
    Entity* entity,
    int turnNumber,
    const glm::ivec2& position,
    int shortStopSteps
) : 
    Action(participant, entity, turnNumber),
    position(position),
    shortStopSteps(shortStopSteps)
{
    game_assert(shortStopSteps >= 0);
}

void MoveAction::publish(ActionPublisher& publisher) {
    publisher.publish<MoveActionEventData>({ turnNumber, entity, position, shortStopSteps });
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
                "[Move]: Failed to validate action, Entity[{}#{}] not engaged but has no path",
                entity->getName(),
                entity->getId()
            );
        }

        return hasPath;
    }

    if(entity->getMovesLeft() <= 0) {
        spdlog::trace(
            "[Move]: Failed to validate action, Entity[{}#{}] has (0/{}) moves left",
            entity->getName(),
            entity->getId(),
            entity->getStats().movesPerTurn
        );
        return false;
    }

    if(getPath().empty()) {
        spdlog::trace(
            "[Move]: Failed to validate action, Entity[{}#{}] has no path",
            entity->getName(),
            entity->getId()
        );
        return false;
    }


    if(!hasAvailableMoves()) {
        spdlog::trace(
            "[Move]: Failed to validate action, Entity[{}#{}] has ({}/{}) moves left but not enough left in chain",
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
    if(participant->getEngagement() == nullptr || !turnNumber.has_value()) {
        return true;
    }

    int numMoves = 0;
 
    for(auto& action : entity->getActionsChain(turnNumber.value())) {
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