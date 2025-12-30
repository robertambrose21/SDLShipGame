#pragma once

#include "game/event/events.h"
#include "game/application/applicationcontext.h"

class ActorController : public EventPublisher<ActorUpdateStatsEventData> {
public:
    ActorController() = default;

    void initialise(ApplicationContext& context);

    void applyDamage(entt::entity entity, uint32_t amount);
    void applyStats(entt::entity entity);

    void useMoves(entt::entity entity, int numMoves);

    void nextTurn(entt::entity entity);
    void endTurn(entt::entity entity);
    bool isTurnInProgress(entt::entity entity);
    void reset(entt::entity entity);

    void engage(entt::entity entity);
    void disengage(entt::entity entity);

    float getSpeed(entt::entity entity);

private:
    ApplicationContext* context;
};