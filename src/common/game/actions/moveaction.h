#pragma once

#include <deque>
#include <algorithm>

#include "game/actors/actor.h"
#include "action.h"

class MoveAction : public Action {
public:
    MoveAction(
        Participant* participant,
        entt::entity entity, 
        const glm::ivec2& position,
        int shortStopSteps = 0
    );
    MoveAction(
        Participant* participant,
        entt::entity entity,
        int turnNumber,
        const glm::ivec2& position,
        int shortStopSteps = 0
    );

    ActionVariant getPublishData(void) override;

    bool passesPrecondition(void);
    Type getType(void);

    glm::ivec2 getPosition(void) const;
    int getShortStopSteps(void) const;

private:
    glm::ivec2 position;
    int shortStopSteps;

    std::deque<glm::ivec2> path;

    bool onValidate(ApplicationContext* context);
    void onExecute(ApplicationContext* context);
    bool hasFinished(ApplicationContext* context);

    std::deque<glm::ivec2> getPath(Actor& actor, bool recalculate = false);
    bool hasAvailableMoves(Actor& actor);
};