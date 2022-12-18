#pragma once

#include <cstdint>
#include "game/entities/entity.h"

class Entity;

class BehaviourStrategy {
protected:
    std::shared_ptr<Entity> owner;

public:
    BehaviourStrategy(const std::shared_ptr<Entity>& owner) :
        owner(owner) { }

    virtual void onUpdate(uint32_t timeSinceLastFrame, bool& quit) = 0;
    virtual void onNextTurn(void) = 0;
    virtual bool endTurnCondition(void) = 0;
};