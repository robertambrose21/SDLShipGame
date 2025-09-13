#include "actorupdatesystem.h"

ActorUpdateSystem::ActorUpdateSystem(const std::string& name) :
    LogicSystem(name)
{ }

void ActorUpdateSystem::update(
    entt::registry& registry,
    ApplicationContext& context,
    int64_t timeSinceLastFrame, 
    bool& quit
) {
    for(auto [entity, actor, stats, position]: registry.view<Actor, Stats::ActorStats, Position>().each()) {
        if(stats.hp <= 0) {
            killActor(context, entity, position);
            continue;
        }

        updateActor(context, entity, actor, stats, position, timeSinceLastFrame);
    }

    for(auto [entity, actor, position]: registry.view<Actor, Position, PositionDirty>().each()) {
        publish<ActorSetPositionEventData>({ entity, position });
        registry.remove<PositionDirty>(entity);
    }
}

void ActorUpdateSystem::updateActor(
    ApplicationContext& context,
    entt::entity entity,
    Actor& actor, 
    const Stats::ActorStats& stats,
    const Position& position, 
    int64_t timeSinceLastFrame
) {
    for(auto weapon : actor.getWeapons()) {
        weapon->update(timeSinceLastFrame);
    }

    if(actor.getIsFrozen()) {
        return;
    }
    
    if(actor.isEngaged() && stats.movesLeft == 0) {
        return;
    }

    if(!actor.hasPath()) {
        return;
    }

    actor.incrementTimeSinceLastMoved(timeSinceLastFrame);

    if(actor.getTimeSinceLastMoved() > context.getActorController()->getSpeed(entity)) {
        auto newPosition = actor.popPath();
        actor.setTimeSinceLastMoved(0);

        if(actor.isEngaged()) {
            context.getActorController()->useMoves(entity, 1);
        }

        context.getEntityRegistry().replace<Position>(entity, newPosition);
        context.getEntityRegistry().get_or_emplace<PositionDirty>(entity);
    }
}

void ActorUpdateSystem::killActor(ApplicationContext& context, entt::entity entity, Position position) {
    publish<ActorEventData>({ entity, position, "Death" });
    context.getActorPool()->removeActor(entity);
}