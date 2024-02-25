#pragma once

#include "core/glmimport.h"
#include "effecttypes.h"
#include "core/grid/grid.h"

class GridEffect {
protected:
    Grid* grid;
    int x, y;
    int duration;

    virtual void doApply(void) = 0;

public:
    GridEffect(Grid* grid, int x, int y, int duration);

    virtual EffectType getType(void) const = 0;

    void apply(void);

    int getDuration(void) const;
    int getX(void) const;
    int getY(void) const;
    void nextTurn(void);
};

class GridFreezeEffect : public GridEffect {
public:
    GridFreezeEffect(Grid* grid, int x, int y, int duration);

    void doApply(void);
    EffectType getType(void) const;
};
