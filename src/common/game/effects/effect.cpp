#include "effect.h"

Effect::Effect(ApplicationContext* context, int duration) :
    context(context),
    duration(duration)
{ }

void Effect::apply(Entity* target) {
    if(duration > 0) {
        doApply(target);
    }
}

void Effect::apply(const glm::ivec2& target) {
    if(duration > 0) {
        doApply(target);
    }
}

int Effect::getDuration(void) const {
    return duration;
}

void Effect::nextTurn(void) {
    duration--;
}



FreezeEffect::FreezeEffect(ApplicationContext* context, int duration) :
    Effect(context, duration),
    direction(glm::ivec2(0, 0))
{ }

void FreezeEffect::doApply(Entity* target) {
    target->setFrozen(true);
}

void FreezeEffect::doApply(const glm::ivec2& target) {
    auto grid = context->getGrid();

    auto perp = glm::normalize(glm::vec2(direction.y, -direction.x));
    auto pX = std::min(grid->getWidth() - 1, (int)std::round(perp.x));
    auto pY = std::min(grid->getHeight() - 1, (int)std::round(perp.y));

    grid->setTileFrozenFor(target.x, target.y, duration);
    grid->setTileFrozenFor(target.x + pX, target.y + pY, duration);
    grid->setTileFrozenFor(target.x - pX, target.y - pY, duration);
}

EffectType FreezeEffect::getType(void) const {
    return EffectType::FREEZE;
}

void FreezeEffect::setDirection(const glm::ivec2& direction) {
    this->direction = direction;
}



PoisonEffect::PoisonEffect(ApplicationContext* context, const std::vector<int>& damageTicks) :
    Effect(context, damageTicks.size()),
    damageTicks(damageTicks),
    totalTicks(damageTicks.size())
{ }

void PoisonEffect::doApply(Entity* target) {
    target->setIsPoisoned(true);
    target->takeDamage(damageTicks[totalTicks - duration]);
    std::cout << "Took " << damageTicks[totalTicks - duration] << " damage" << std::endl;
}

void PoisonEffect::doApply(const glm::ivec2& target) {
    // no-op
}

EffectType PoisonEffect::getType(void) const {
    return EffectType::POISON;
}

const std::vector<int>& PoisonEffect::getDamageTicks(void) const {
    return damageTicks;
}