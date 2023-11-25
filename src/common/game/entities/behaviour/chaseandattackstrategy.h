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
    ApplicationContext& context;
    GameServerMessagesTransmitter* transmitter;

    bool canPassTurn;

    Entity* findClosestTarget(Entity* attacker);
    Weapon* getBestInRangeWeapon(Entity* attacker, const glm::ivec2& target);
    bool doTurnForEntity(Entity* entity);

public:
    ChaseAndAttackStrategy(ApplicationContext& context, int participantId);

    void onUpdate(int64_t timeSinceLastFrame, bool& quit);
    void onNextTurn(void);
    bool endTurnCondition(void);
};