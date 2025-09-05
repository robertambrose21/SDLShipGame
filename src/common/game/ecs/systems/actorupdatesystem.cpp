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
    for(auto [entity, actor, position]: registry.view<Actor, Position>().each()) {
        if(actor.getCurrentHP() <= 0) {
            killActor(context, entity, actor, position);
            continue;
        }

        updateActor(context, entity, actor, position, timeSinceLastFrame);
    }

    for(auto [entity, actor, position]: registry.view<Actor, Position, PositionDirty>().each()) {
        publish<ActorSetPositionEventData>({ &actor, position });
        registry.remove<PositionDirty>(entity);
    }
}

void ActorUpdateSystem::updateActor(
    ApplicationContext& context,
    entt::entity entity,
    Actor& actor, 
    Position& position, 
    int64_t timeSinceLastFrame
) {
    for(auto weapon : actor.getWeapons()) {
        weapon->update(timeSinceLastFrame);
    }

    if(actor.getIsFrozen()) {
        return;
    }
    
    if(actor.isEngaged() && actor.getMovesLeft() == 0) {
        return;
    }

    if(!actor.hasPath()) {
        return;
    }

    actor.incrementTimeSinceLastMoved(timeSinceLastFrame);

    if(actor.getTimeSinceLastMoved() > actor.getSpeed()) {
        auto position = actor.popPath();
        actor.setTimeSinceLastMoved(0);

        if(actor.isEngaged()) {
            actor.useMoves(1);
        }

        context.getEntityRegistry().replace<Position>(entity, position);
        context.getEntityRegistry().get_or_emplace<PositionDirty>(entity);
    }
}

void ActorUpdateSystem::killActor(ApplicationContext& context, entt::entity, Actor& actor, Position position) {
    context.getActorPool()->removeActor(actor.getId());
    publish<ActorEventData>({ &actor, position, "Death" });
}