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
    std::shared_ptr<TurnController> turnController;

    std::map<TurnController::Action, int> presetActions;

    bool canPassTurn;

    std::shared_ptr<Entity> findClosestTarget(const std::shared_ptr<Entity>& attacker);
    std::shared_ptr<Weapon> getBestInRangeWeapon(const std::shared_ptr<Entity>& attacker, const glm::ivec2& target);

public:
    ChaseAndAttackStrategy(int participantId);

    void onUpdate(uint32_t timeSinceLastFrame, bool& quit);
    void onNextTurn(void);
    bool endTurnCondition(void);
};