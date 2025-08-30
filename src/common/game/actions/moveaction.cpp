#include "moveaction.h"
#include "game/participant/participant.h"

MoveAction::MoveAction(
    Participant* participant,
    entt::entity entity, 
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
    entt::entity entity,
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

ActionVariant MoveAction::getPublishData(void) {
    return MoveActionEventData { turnNumber, entity, position, shortStopSteps };
}

Action::Type MoveAction::getType(void) {
    return Action::Type::Move;
}

bool MoveAction::passesPrecondition(void) {
    return true;
}

bool MoveAction::onValidate(ApplicationContext* context) {
    auto actor = context->getEntityRegistry().try_get<Actor>(entity);

    if(!actor) {
        spdlog::trace("[{}]: Failed to validate action, actor is null", typeToString());
        return false;
    }

    if(!actor->isEngaged()) {
        auto hasPath = !getPath(context).empty();

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

    if(getPath(context).empty()) {
        spdlog::trace(
            "[Move]: Failed to validate action, Actor[{}#{}] has no path",
            actor->getName(),
            actor->getId()
        );
        return false;
    }


    if(!hasAvailableMoves(context, actor)) {
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
    auto actor = context->getEntityRegistry().try_get<Actor>(entity);

    if(!actor) {
        spdlog::trace("[{}]: Failed to execute action, actor is null", typeToString());
        return;
    }

    actor->setPath(getPath(context, true));
}

bool MoveAction::hasFinished(ApplicationContext* context) {
    auto actor = context->getEntityRegistry().try_get<Actor>(entity);

    if(!actor) {
        spdlog::trace("[{}]: Failed to finish action, actor is null", typeToString());
        return false;
    }

    if(actor->getMovesLeft() <= 0) {
        return true;
    }

    if(path.empty()) {
        return true;
    }

    return actor->getPosition() == path[std::min(path.size() - shortStopSteps - 1, 0UL)];
}

std::deque<glm::ivec2> MoveAction::getPath(ApplicationContext* context, bool recalculate) {
    if(recalculate || path.empty()) {
        path = calculatePath(context, position, shortStopSteps);
    }

    return path;
}

std::deque<glm::ivec2> MoveAction::calculatePath(
    ApplicationContext* context,
    const glm::ivec2& target, 
    int stopShortSteps
) {
    // TODO: Get position instead eventually
    auto actor = context->getEntityRegistry().try_get<Actor>(entity);

    if(!actor) {
        spdlog::trace("[{}]: Failed to calculate path, actor is null", typeToString());
        return std::deque<glm::ivec2>();
    }

    auto path = context->getGrid()->findPath(actor->getPosition(), target);

    if(path.empty()) {
        return std::deque<glm::ivec2>();
    }

    // Remove the initial path node which is just the actors current position
    path.pop_front(); 

    if(path.size() >= stopShortSteps) {
        for(auto i = 0; i < stopShortSteps; i++) {
            path.pop_back();
        }
    }

    return path;
}

bool MoveAction::hasAvailableMoves(ApplicationContext* context, Actor* actor) {
    if(participant->getEngagement() == nullptr || !turnNumber.has_value()) {
        return true;
    }

    int numMoves = 0;
 
    for(auto& action : actor->getActionsChain(turnNumber.value())) {
        if(action->getType() == Action::Type::Move) {
            numMoves += dynamic_cast<MoveAction*>(action)->getPath(context).size();
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