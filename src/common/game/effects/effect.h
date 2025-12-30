#pragma once

#include <vector>

#include "game/actors/actor.h"
#include "effecttypes.h"
#include "game/stats/stats.h"
#include "game/actors/actorcontroller.h"

class Effect {
public:
    static const int64_t RealTimeTick = 1000;

    Effect(entt::entity target, int ownerId, const Stats::EffectStats& stats);

    virtual EffectType getType(void) const = 0;

    void apply(ApplicationContext* context);
    void update(int64_t timeSinceLastFrame);

    Stats::EffectStats getStats(void) const;
    uint8_t getTicksLeft(void) const;
    void nextTurn(void);

    entt::entity getTarget(void);
    int getOwnerId(void) const;

    int64_t getTimeSinceLastTick(void) const;

    virtual void onEffectEnd(ApplicationContext* context) = 0;

protected:
    entt::entity target;
    uint8_t ticksLeft;
    Stats::EffectStats stats;
    int ownerId;

    int64_t timeSinceLastTick;

    virtual void doApply(ApplicationContext* context) = 0;
};

class FreezeEffect : public Effect {
public:
    FreezeEffect(entt::entity target, int ownerId, const Stats::EffectStats& stats);

    void doApply(ApplicationContext* context);
    void onEffectEnd(ApplicationContext* context);
    EffectType getType(void) const;
};

class PoisonEffect : public Effect {
public:
    PoisonEffect(entt::entity target, int ownerId, const Stats::EffectStats& stats);

    void doApply(ApplicationContext* context);
    void onEffectEnd(ApplicationContext* context);
    EffectType getType(void) const;
};
