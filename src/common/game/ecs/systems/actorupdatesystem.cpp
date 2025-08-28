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
    for(auto [entityId, actor]: registry.view<Actor>().each()) {
        if(actor.getCurrentHP() <= 0) {
            killActor(context, actor);
            continue;
        }

        updateActor(actor, timeSinceLastFrame);
    }
}

void ActorUpdateSystem::updateActor(Actor& actor, int64_t timeSinceLastFrame) {
    bool quit = false;
    actor.update(timeSinceLastFrame, quit);
}

void ActorUpdateSystem::killActor(ApplicationContext& context, Actor& actor) {
    context.getActorPool()->removeActor(actor.getId());
    publish<ActorEventData>({ &actor, "Death" });
}