#pragma once

#include "behaviourstrategy.h"
#include "game/actors/actor.h"
#include "game/actors/actorpool.h"
#include "game/application/application.h"
#include "application/net/gameservermessagestransmitter.h"
#include "game/actions/action.h"
#include "game/actions/moveaction.h"
#include "game/actions/attackaction.h"

#include <thread>

class GameServerMessagesTransmitter;

class ChaseAndAttackStrategy : public BehaviourStrategy {
private:
    typedef struct _actorTurnResult {
        bool canPass, canDisengage;
    } ActorTurnResult;

    GameServerMessagesTransmitter* transmitter;

    bool canPassTurn;
    bool canDisengage;

    Weapon* getBestInRangeWeapon(Actor* attacker, const glm::ivec2& target);
    ActorTurnResult doTurnForActor(Actor* actor, Participant* participant);

public:
    ChaseAndAttackStrategy(ApplicationContext& context);

    void onUpdate(int participantId, int64_t timeSinceLastFrame, bool& quit);
    void onNextTurn(void);
    bool endTurnCondition(void);
    bool disengageCondition(void);
};