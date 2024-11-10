#pragma once

#include <vector>

#include "game/entities/entity.h"
#include "effecttypes.h"
#include "game/stats/stats.h"

class Effect {
protected:
    Entity* target;
    int ticksLeft;
    EffectStats2 stats;

    virtual void doApply(void) = 0;

public:
    Effect(Entity* target, const EffectStats2& stats);

    virtual EffectType getType(void) const = 0;

    void apply(void);

    EffectStats2 getStats(void) const;
    int getTicksLeft(void) const;
    void nextTurn(void);

    Entity* getTarget(void);
};

class FreezeEffect : public Effect {
public:
    FreezeEffect(Entity* target, const EffectStats2& stats);

    void doApply(void);
    EffectType getType(void) const;
};

class PoisonEffect : public Effect {
public:
    PoisonEffect(Entity* target, const EffectStats2& stats);

    void doApply(void);
    EffectType getType(void) const;
};
