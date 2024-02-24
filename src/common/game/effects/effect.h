#pragma once

#include <vector>

#include "game/entities/entity.h"
#include "game/application/applicationcontext.h"
#include "effecttypes.h"

class Effect {
protected:
    int duration;
    ApplicationContext* context;

    virtual void doApply(Entity* target) = 0;
    virtual void doApply(const glm::ivec2& target) = 0;

public:
    Effect(ApplicationContext* context, int duration);

    virtual EffectType getType(void) const = 0;

    void apply(Entity* target);
    void apply(const glm::ivec2& target);

    int getDuration(void) const;
    void nextTurn(void);
};

class FreezeEffect : public Effect {
private:
    glm::ivec2 direction;

public:
    FreezeEffect(ApplicationContext* context, int duration);

    void doApply(Entity* target);
    void doApply(const glm::ivec2& target);
    EffectType getType(void) const;

    void setDirection(const glm::ivec2& direction);
};

class PoisonEffect : public Effect {
private:
    std::vector<int> damageTicks;
    int totalTicks;

public:
    PoisonEffect(ApplicationContext* context, const std::vector<int>& damageTicks);

    void doApply(Entity* target);
    void doApply(const glm::ivec2& target);
    EffectType getType(void) const;

    const std::vector<int>& getDamageTicks(void) const;
};
