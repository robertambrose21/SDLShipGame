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
            killActor(context, actor);
            continue;
        }

        updateActor(context, actor, position, timeSinceLastFrame);
    }
}

void ActorUpdateSystem::updateActor(
    ApplicationContext& context,
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

        context.getActorPool()->setPosition(actor.getId(), position);
    }
}

void ActorUpdateSystem::killActor(ApplicationContext& context, Actor& actor) {
    auto const& position = context.getActorPool()->getPosition(actor.getId());
    context.getActorPool()->removeActor(actor.getId());
    publish<ActorEventData>({ &actor, position, "Death" });
}