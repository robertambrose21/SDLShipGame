#include "grideffect.h"

GridEffect::GridEffect(Grid* grid, int x, int y, int duration) :
    grid(grid),
    x(x),
    y(y),
    duration(duration)
{ }

void GridEffect::apply(void) {
    if(duration > 0) {
        doApply();
    }
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
}



GridFreezeEffect::GridFreezeEffect(Grid* grid, int x, int y, int duration) :
    GridEffect(grid, x, y, duration)
{ }

void GridFreezeEffect::doApply(void) {
    grid->setTileFrozen(x, y, true);
}

EffectType GridFreezeEffect::getType(void) const {
    return EffectType::FREEZE;
}