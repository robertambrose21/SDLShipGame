#pragma once

#include <deque>

#include "game/entities/entity.h"
#include "action.h"

class MoveAction : public Action {
private:
    glm::ivec2 position;
    int shortStopSteps;

    std::deque<glm::ivec2> path;

    bool onValidate(ApplicationContext* context);
    void onExecute(ApplicationContext* context);
    bool hasFinished(void);

    std::deque<glm::ivec2> getPath(bool recalculate = false);
    bool hasAvailableMoves(void);

public:
    MoveAction(
        int turnNumber,
        Entity* entity, 
        const glm::ivec2& position,
        int shortStopSteps = 0
    );

    bool passesPrecondition(void);
    Type getType(void);

    glm::ivec2 getPosition(void) const;
    int getShortStopSteps(void) const;
};