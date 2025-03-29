#include "effect.h"

Effect::Effect(Entity* target, int ownerId, const Stats::EffectStats& stats) :
    target(target),
    ownerId(ownerId),
    stats(stats),
    ticksLeft(stats.duration),
    timeSinceLastTick(0)
{ }

void Effect::apply(void) {
    if(ticksLeft > 0) {
        doApply();
    }
}

void Effect::update(int64_t timeSinceLastFrame) {
    timeSinceLastTick += timeSinceLastFrame;
}

Stats::EffectStats Effect::getStats(void) const {
    return stats;
}

uint8_t Effect::getTicksLeft(void) const {
    return ticksLeft;
}

void Effect::nextTurn(void) {
    spdlog::info("Effect tick ({}/{})", ticksLeft, stats.duration);
    ticksLeft--;
    timeSinceLastTick = 0;
}

Entity* Effect::getTarget(void) {
    return target;
}

int Effect::getOwnerId(void) const {
    return ownerId;
}

int64_t Effect::getTimeSinceLastTick(void) const {
    return timeSinceLastTick;
}



FreezeEffect::FreezeEffect(Entity* target, int ownerId, const Stats::EffectStats& stats) :
    Effect(target, ownerId, stats)
{ }

void FreezeEffect::doApply(void) {
    target->setFrozen(true);
}

EffectType FreezeEffect::getType(void) const {
    return EffectType::FREEZE;
}



PoisonEffect::PoisonEffect(Entity* target, int ownerId, const Stats::EffectStats& stats) :
    Effect(target, ownerId, stats)
{ }

void PoisonEffect::doApply(void) {
    game_assert(stats.duration - ticksLeft >= 0);
    target->setIsPoisoned(true);
    target->takeDamage(stats.damageTicks[stats.duration - ticksLeft]);
}

EffectType PoisonEffect::getType(void) const {
    return EffectType::POISON;
}