#pragma once

#include <vector>

#include "game/entities/entity.h"
#include "effecttypes.h"
#include "game/stats/stats.h"

class Effect {
public:
    static const int64_t RealTimeTick = 1000;

    Effect(Entity* target, int ownerId, const Stats::EffectStats& stats);

    virtual EffectType getType(void) const = 0;

    void apply(void);
    void update(int64_t timeSinceLastFrame);

    Stats::EffectStats getStats(void) const;
    uint8_t getTicksLeft(void) const;
    void nextTurn(void);

    Entity* getTarget(void);
    int getOwnerId(void) const;

    int64_t getTimeSinceLastTick(void) const;

protected:
    Entity* target;
    uint8_t ticksLeft;
    Stats::EffectStats stats;
    int ownerId;

    int64_t timeSinceLastTick;

    virtual void doApply(void) = 0;
};

class FreezeEffect : public Effect {
public:
    FreezeEffect(Entity* target, int ownerId, const Stats::EffectStats& stats);

    void doApply(void);
    EffectType getType(void) const;
};

class PoisonEffect : public Effect {
public:
    PoisonEffect(Entity* target, int ownerId, const Stats::EffectStats& stats);

    void doApply(void);
    EffectType getType(void) const;
};
