#pragma once

#include "behaviourstrategy.h"
#include "game/entities/entity.h"
#include "game/entities/entitypool.h"
#include "game/application/application.h"
#include "game/net/server/gameservermessagestransmitter.h"

class GameServerMessagesTransmitter;

class ChaseAndAttackStrategy : public BehaviourStrategy {
private:
    std::shared_ptr<GameServerMessagesTransmitter> transmitter;

    std::shared_ptr<EntityPool> entityPool;
    std::shared_ptr<Entity> target;

    bool canPassTurn;

    std::shared_ptr<Entity> findClosestTarget(void);

public:
    ChaseAndAttackStrategy(std::shared_ptr<Entity> owner);

    void onUpdate(const uint32_t& timeSinceLastFrame, bool& quit);
    void onNextTurn(void);
    bool endTurnCondition(void);
};