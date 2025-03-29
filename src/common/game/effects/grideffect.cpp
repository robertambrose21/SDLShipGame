#include "grideffect.h"

GridEffect::GridEffect(Grid* grid, int ownerId, int x, int y, int duration) :
    grid(grid),
    ownerId(ownerId),
    x(x),
    y(y),
    duration(duration),
    timeSinceLastTick(0)
{ }

void GridEffect::apply(void) {
    if(duration > 0) {
        doApply();
    }
}

void GridEffect::update(int64_t timeSinceLastFrame) {
    timeSinceLastTick += timeSinceLastFrame;
}

int GridEffect::getDuration(void) const {
    return duration;
}

int GridEffect::getX(void) const {
    return x;
}

int GridEffect::getY(void) const {
    return y;
}

void GridEffect::nextTurn(void) {
    duration--;
    timeSinceLastTick = 0;
}

int GridEffect::getOwnerId(void) const {
    return ownerId;
}

int64_t GridEffect::getTimeSinceLastTick(void) const {
    return timeSinceLastTick;
}




GridFreezeEffect::GridFreezeEffect(Grid* grid, int ownerId, int x, int y, int duration) :
    GridEffect(grid, ownerId, x, y, duration)
{ }

void GridFreezeEffect::doApply(void) {
    grid->setTileFrozen(x, y, true);
}

void GridFreezeEffect::onEffectEnd(void) {
    grid->setTileFrozen(x, y, false);
}

EffectType GridFreezeEffect::getType(void) const {
    return EffectType::FREEZE;
}