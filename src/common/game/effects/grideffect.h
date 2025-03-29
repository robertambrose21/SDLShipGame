#pragma once

#include "core/glmimport.h"
#include "effecttypes.h"
#include "core/grid/grid.h"

class GridEffect {
public:
    static const int64_t RealTimeTick = 1000;

    GridEffect(Grid* grid, int ownerId, int x, int y, int duration);

    virtual EffectType getType(void) const = 0;

    void apply(void);
    void update(int64_t timeSinceLastFrame);

    int getDuration(void) const;
    int getX(void) const;
    int getY(void) const;
    void nextTurn(void);

    int getOwnerId(void) const;

    int64_t getTimeSinceLastTick(void) const;

    virtual void onEffectEnd(void) = 0;

protected:
    Grid* grid;
    int ownerId;
    int x, y;
    int duration;

    int64_t timeSinceLastTick;

    virtual void doApply(void) = 0;
};

class GridFreezeEffect : public GridEffect {
public:
    GridFreezeEffect(Grid* grid, int ownerId, int x, int y, int duration);

    void doApply(void);
    void onEffectEnd(void);
    
    EffectType getType(void) const;
};
