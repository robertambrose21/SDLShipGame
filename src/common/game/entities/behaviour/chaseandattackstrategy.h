#pragma once

#include "behaviourstrategy.h"
#include "game/entities/entity.h"
#include "game/entities/entitypool.h"
#include "game/application/application.h"
#include "application/net/gameservermessagestransmitter.h"
#include "game/actions/action.h"
#include "game/actions/moveaction.h"
#include "game/actions/attackaction.h"

#include <thread>

class GameServerMessagesTransmitter;

class ChaseAndAttackStrategy : public BehaviourStrategy {
private:
    typedef struct _entityTurnResult {
        bool canPass, canDisengage;
    } EntityTurnResult;

    GameServerMessagesTransmitter* transmitter;

    bool canPassTurn;
    bool canDisengage;

    Weapon* getBestInRangeWeapon(Entity* attacker, const glm::ivec2& target);
    EntityTurnResult doTurnForEntity(Entity* entity, Participant* participant);

public:
    ChaseAndAttackStrategy(ApplicationContext& context);

    void onUpdate(int participantId, int64_t timeSinceLastFrame, bool& quit);
    void onNextTurn(void);
    bool endTurnCondition(void);
    bool disengageCondition(void);
};