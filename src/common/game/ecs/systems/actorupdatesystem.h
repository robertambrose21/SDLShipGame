#pragma once

#include <vector>

#include "game/ecs/logicsystemregistry.h"
#include "game/actors/actor.h"
#include "game/application/gamecontroller.h"
#include "core/event/eventpublisher.h"

class ActorUpdateSystem :
    public LogicSystem, 
    public EventPublisher<ActorEventData, ActorSetPositionEventData, ActorUpdateStatsEventData> 
{
public:
    ActorUpdateSystem() = delete;
    ActorUpdateSystem(const std::string& name);

    void update(
        entt::registry& registry, 
        ApplicationContext& context, 
        int64_t timeSinceLastFrame, 
        bool& quit
    );

private:
    void updateActor(Actor& actor, int64_t timeSinceLastFrame);
    void killActor(ApplicationContext& context, Actor& actor);
};