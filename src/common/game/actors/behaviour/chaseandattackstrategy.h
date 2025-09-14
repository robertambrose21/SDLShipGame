#pragma once

#include <thread>
#include <optional>

#include "behaviourstrategy.h"
#include "game/actors/actor.h"
#include "game/actors/actorpool.h"
#include "game/application/application.h"
#include "application/net/gameservermessagestransmitter.h"
#include "game/actions/action.h"
#include "game/actions/moveaction.h"
#include "game/actions/attackaction.h"

class GameServerMessagesTransmitter;

class ChaseAndAttackStrategy : public BehaviourStrategy {
private:
    typedef struct _actorTurnResult {
        bool canPass, canDisengage;
    } ActorTurnResult;

    GameServerMessagesTransmitter* transmitter;

    bool canPassTurn;
    bool canDisengage;

    std::optional<entt::entity> getBestInRangeWeapon(Actor* attacker, const glm::ivec2& target);
    ActorTurnResult doTurnForActor(entt::entity entity, Participant* participant);

public:
    ChaseAndAttackStrategy(ApplicationContext& context);

    void onUpdate(int participantId, int64_t timeSinceLastFrame, bool& quit);
    void onNextTurn(void);
    bool endTurnCondition(void);
    bool disengageCondition(void);
};