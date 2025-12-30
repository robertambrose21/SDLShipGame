#include "effect.h"

Effect::Effect(entt::entity target, int ownerId, const Stats::EffectStats& stats) :
    target(target),
    ownerId(ownerId),
    stats(stats),
    ticksLeft(stats.duration),
    timeSinceLastTick(0)
{ }

void Effect::apply(ApplicationContext* context) {
    if(ticksLeft > 0) {
        doApply(context);
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
    // spdlog::info("Effect tick ({}/{})", ticksLeft, stats.duration);
    ticksLeft--;
    timeSinceLastTick = 0;
}

entt::entity Effect::getTarget(void) {
    return target;
}

int Effect::getOwnerId(void) const {
    return ownerId;
}

int64_t Effect::getTimeSinceLastTick(void) const {
    return timeSinceLastTick;
}



FreezeEffect::FreezeEffect(entt::entity target, int ownerId, const Stats::EffectStats& stats) :
    Effect(target, ownerId, stats)
{ }

void FreezeEffect::doApply(ApplicationContext* context) {
    auto& actor = context->getEntityRegistry().get<Actor>(target);
    actor.setFrozen(true);
}

EffectType FreezeEffect::getType(void) const {
    return EffectType::FREEZE;
}

void FreezeEffect::onEffectEnd(ApplicationContext* context) {
    auto& actor = context->getEntityRegistry().get<Actor>(target);
    actor.setFrozen(false);
}


PoisonEffect::PoisonEffect(entt::entity target, int ownerId, const Stats::EffectStats& stats) :
    Effect(target, ownerId, stats)
{ }

void PoisonEffect::doApply(ApplicationContext* context) {
    game_assert(stats.duration - ticksLeft >= 0);
    auto& actor = context->getEntityRegistry().get<Actor>(target);

    actor.setIsPoisoned(true);
    context->getActorController()->applyDamage(target, stats.damageTicks[stats.duration - ticksLeft]);
}

EffectType PoisonEffect::getType(void) const {
    return EffectType::POISON;
}

void PoisonEffect::onEffectEnd(ApplicationContext* context) {
    auto& actor = context->getEntityRegistry().get<Actor>(target);
    actor.setIsPoisoned(false);
}