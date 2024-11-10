#include "effect.h"

Effect::Effect(Entity* target, const Stats::EffectStats& stats) :
    target(target),
    stats(stats),
    ticksLeft(stats.duration)
{ }

void Effect::apply(void) {
    if(ticksLeft > 0) {
        doApply();
    }
}

Stats::EffectStats Effect::getStats(void) const {
    return stats;
}

int Effect::getTicksLeft(void) const {
    return ticksLeft;
}

void Effect::nextTurn(void) {
    ticksLeft--;
}

Entity* Effect::getTarget(void) {
    return target;
}



FreezeEffect::FreezeEffect(Entity* target, const Stats::EffectStats& stats) :
    Effect(target, stats)
{ }

void FreezeEffect::doApply(void) {
    target->setFrozen(true);
}

EffectType FreezeEffect::getType(void) const {
    return EffectType::FREEZE;
}



PoisonEffect::PoisonEffect(Entity* target, const Stats::EffectStats& stats) :
    Effect(target, stats)
{ }

void PoisonEffect::doApply(void) {
    target->setIsPoisoned(true);
    target->takeDamage(stats.damageTicks[stats.duration - ticksLeft - 1]);
}

EffectType PoisonEffect::getType(void) const {
    return EffectType::POISON;
}