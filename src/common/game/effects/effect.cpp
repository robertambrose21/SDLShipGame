#include "effect.h"

Effect::Effect(Entity* target, int duration) :
    target(target),
    duration(duration)
{ }

void Effect::apply(void) {
    if(duration > 0) {
        doApply();
    }
}

int Effect::getDuration(void) const {
    return duration;
}

void Effect::nextTurn(void) {
    duration--;
}

Entity* Effect::getTarget(void) {
    return target;
}



FreezeEffect::FreezeEffect(Entity* target, int duration) :
    Effect(target, duration),
    direction(glm::ivec2(0, 0))
{ }

void FreezeEffect::doApply(void) {
    target->setFrozen(true);
}

EffectType FreezeEffect::getType(void) const {
    return EffectType::FREEZE;
}

void FreezeEffect::setDirection(const glm::ivec2& direction) {
    this->direction = direction;
}



PoisonEffect::PoisonEffect(Entity* target, const std::vector<int>& damageTicks) :
    Effect(target, damageTicks.size()),
    damageTicks(damageTicks),
    totalTicks(damageTicks.size())
{ }

void PoisonEffect::doApply(void) {
    target->setIsPoisoned(true);
    target->takeDamage(damageTicks[totalTicks - duration]);
}

EffectType PoisonEffect::getType(void) const {
    return EffectType::POISON;
}

const std::vector<int>& PoisonEffect::getDamageTicks(void) const {
    return damageTicks;
}