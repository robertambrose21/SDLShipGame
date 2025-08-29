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
    auto& actor = context->getEntityRegistry().get<Actor>(entity);

    if(!actor.isEngaged()) {
        auto hasPath = !getPath(actor).empty();

        if(!hasPath) {
            spdlog::trace(
                "[Move]: Failed to validate action, Actor[{}#{}] not engaged but has no path",
                actor.getName(),
                actor.getId()
            );
        }

        return hasPath;
    }

    if(actor.getMovesLeft() <= 0) {
        spdlog::trace(
            "[Move]: Failed to validate action, Actor[{}#{}] has (0/{}) moves left",
            actor.getName(),
            actor.getId(),
            actor.getStats().movesPerTurn
        );
        return false;
    }

    if(getPath(actor).empty()) {
        spdlog::trace(
            "[Move]: Failed to validate action, Actor[{}#{}] has no path",
            actor.getName(),
            actor.getId()
        );
        return false;
    }


    if(!hasAvailableMoves(actor)) {
        spdlog::trace(
            "[Move]: Failed to validate action, Actor[{}#{}] has ({}/{}) moves left but not enough left in chain",
            actor.getName(),
            actor.getId(),
            actor.getMovesLeft(),
            actor.getStats().movesPerTurn
        );
        return false;
    }

    return true;
}

void MoveAction::onExecute(ApplicationContext* context) {
    auto& actor = context->getEntityRegistry().get<Actor>(entity);
    actor.setPath(getPath(actor, true));
}

bool MoveAction::hasFinished(ApplicationContext* context) {
    auto& actor = context->getEntityRegistry().get<Actor>(entity);

    if(actor.getMovesLeft() <= 0) {
        return true;
    }

    if(path.empty()) {
        return true;
    }

    return actor.getPosition() == path[std::min(path.size() - shortStopSteps - 1, 0UL)];
}

std::deque<glm::ivec2> MoveAction::getPath(Actor& actor, bool recalculate) {
    if(recalculate || path.empty()) {
        path = actor.calculatePath(position, shortStopSteps);
    }

    return path;
}

bool MoveAction::hasAvailableMoves(Actor& actor) {
    if(participant->getEngagement() == nullptr || !turnNumber.has_value()) {
        return true;
    }

    int numMoves = 0;
 
    for(auto& action : actor.getActionsChain(turnNumber.value())) {
        if(action->getType() == Action::Type::Move) {
            numMoves += dynamic_cast<MoveAction*>(action)->getPath(actor).size();
        }
    }

    return actor.getMovesLeft() >= numMoves;
}

glm::ivec2 MoveAction::getPosition(void) const {
    return position;
}

int MoveAction::getShortStopSteps(void) const {
    return shortStopSteps;
}