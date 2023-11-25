#pragma once

#include <deque>

#include "game/entities/entity.h"
#include "action.h"

class MoveAction : public Action {
private:
    glm::ivec2 position;
    int shortStopSteps;

    std::deque<glm::ivec2> path;

    bool onValidate(void);
    void onExecute(ApplicationContext* context);
    bool hasFinished(void);

    std::deque<glm::ivec2> getPath(bool recalculate = false);
    bool hasAvailableMoves(void);

public:
    MoveAction(
        Entity* entity, 
        const glm::ivec2& position,
        int shortStopSteps = 0
    );

    Type getType(void);
};