#pragma once

#include <vector>

#include "game/ecs/logicsystemregistry.h"
#include "game/actors/actor.h"
#include "game/application/gamecontroller.h"
#include "core/event/eventpublisher.h"

class ActorUpdateSystem :
    public LogicSystem, 
    public EventPublisher<ActorSetPositionEventData, ActorEventData> 
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
    void updateActor(
        ApplicationContext& context,
        entt::entity,
        Actor& actor,
        const Stats::ActorStats& stats,
        const Position& position, 
        int64_t timeSinceLastFrame
    );
    void killActor(ApplicationContext& context, entt::entity entity, Position position);
    float getActorSpeed(const Stats::ActorStats& stats);
};