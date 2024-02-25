#pragma once

#include <vector>

#include "game/entities/entity.h"
#include "effecttypes.h"

class Effect {
protected:
    Entity* target;
    int duration;

    virtual void doApply(void) = 0;

public:
    Effect(Entity* target, int duration);

    virtual EffectType getType(void) const = 0;

    void apply(void);

    int getDuration(void) const;
    void nextTurn(void);

    Entity* getTarget(void);
};

class FreezeEffect : public Effect {
private:
    glm::ivec2 direction;

public:
    FreezeEffect(Entity* target, int duration);

    void doApply(void);
    EffectType getType(void) const;

    void setDirection(const glm::ivec2& direction);
};

class PoisonEffect : public Effect {
private:
    std::vector<int> damageTicks;
    int totalTicks;

public:
    PoisonEffect(Entity* target, const std::vector<int>& damageTicks);

    void doApply(void);
    EffectType getType(void) const;

    const std::vector<int>& getDamageTicks(void) const;
};
