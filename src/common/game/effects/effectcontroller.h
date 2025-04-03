#pragma once

#include <map>
#include <vector>

#include "effect.h"
#include "grideffect.h"
#include "game/event/events.h"
#include "core/event/eventpublisher.h"
#include "game/application/applicationcontext.h"
#include "game/application/gamecontroller.h"

class EffectController : public EventPublisher<EntityEffectEvent, GridEffectEvent> {
public:
    EffectController();

    void initialise(ApplicationContext& context);
    void update(int64_t timeSinceLastFrame);

    Effect* addEffect(std::unique_ptr<Effect> effect);
    GridEffect* addGridEffect(std::unique_ptr<GridEffect> effect);

private:
    std::map<uint32_t, std::vector<std::unique_ptr<Effect>>> engagementEffects;
    std::vector<std::unique_ptr<Effect>> adhocEffects;
    std::map<uint32_t, std::vector<std::unique_ptr<GridEffect>>> engagementGridEffects;
    std::vector<std::unique_ptr<GridEffect>> adhocGridEffects;

    void updateEngagementEffects(int64_t timeSinceLastFrame);
    void updateAdhocEffects(int64_t timeSinceLastFrame);

    ApplicationContext* context;
    bool initialised;
};
