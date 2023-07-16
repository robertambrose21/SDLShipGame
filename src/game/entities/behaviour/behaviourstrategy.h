#pragma once

#include <cstdint>
// #include "game/entities/entity.h"

// class Entity;

class BehaviourStrategy {
protected:
    // Entity* owner;
    int participantId;

public:
    BehaviourStrategy(int participantId) :
        participantId(participantId) { }

    virtual void onUpdate(uint32_t timeSinceLastFrame, bool& quit) = 0;
    virtual void onNextTurn(void) = 0;
    virtual bool endTurnCondition(void) = 0;
};