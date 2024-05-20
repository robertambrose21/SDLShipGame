#pragma once

#include <cstdint>
#include "game/entities/entity.h"
#include "game/entities/entitypool.h"
#include "game/application/applicationcontext.h"

class BehaviourStrategy {
private:
    ApplicationContext& context;

public:
    BehaviourStrategy(ApplicationContext& context);

    virtual void onUpdate(int participantId, int64_t timeSinceLastFrame, bool& quit) = 0;
    virtual void onNextTurn(void) = 0;
    virtual bool endTurnCondition(void) = 0;
    virtual bool disengageCondition(void) = 0;

    ApplicationContext& getContext(void);
};