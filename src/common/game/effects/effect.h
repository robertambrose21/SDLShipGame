#pragma once

#include <vector>

#include "game/actors/actor.h"
#include "effecttypes.h"
#include "game/stats/stats.h"

class Effect {
public:
    static const int64_t RealTimeTick = 1000;

    Effect(Actor* target, int ownerId, const Stats::EffectStats& stats);

    virtual EffectType getType(void) const = 0;

    void apply(void);
    void update(int64_t timeSinceLastFrame);

    Stats::EffectStats getStats(void) const;
    uint8_t getTicksLeft(void) const;
    void nextTurn(void);

    Actor* getTarget(void);
    int getOwnerId(void) const;

    int64_t getTimeSinceLastTick(void) const;

    virtual void onEffectEnd(void) = 0;

protected:
    Actor* target;
    uint8_t ticksLeft;
    Stats::EffectStats stats;
    int ownerId;

    int64_t timeSinceLastTick;

    virtual void doApply(void) = 0;
};

class FreezeEffect : public Effect {
public:
    FreezeEffect(Actor* target, int ownerId, const Stats::EffectStats& stats);

    void doApply(void);
    void onEffectEnd(void);
    EffectType getType(void) const;
};

class PoisonEffect : public Effect {
public:
    PoisonEffect(Actor* target, int ownerId, const Stats::EffectStats& stats);

    void doApply(void);
    void onEffectEnd(void);
    EffectType getType(void) const;
};
