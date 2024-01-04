#pragma once

#include <cstdint>

class BehaviourStrategy {
public:
    virtual void onUpdate(int participantId, int64_t timeSinceLastFrame, bool& quit) = 0;
    virtual void onNextTurn(void) = 0;
    virtual bool endTurnCondition(void) = 0;
};