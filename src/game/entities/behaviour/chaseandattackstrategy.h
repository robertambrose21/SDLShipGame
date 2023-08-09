#pragma once

#include "behaviourstrategy.h"
#include "game/entities/entity.h"
#include "game/entities/entitypool.h"
#include "game/application/application.h"
#include "game/net/server/gameservermessagestransmitter.h"

class GameServerMessagesTransmitter;

class ChaseAndAttackStrategy : public BehaviourStrategy {
private:
    GameServerMessagesTransmitter* transmitter;

    std::map<TurnController::Action, int> presetActions;

    bool canPassTurn;

    Entity* findClosestTarget(Entity* attacker);
    Weapon* getBestInRangeWeapon(Entity* attacker, const glm::ivec2& target);

public:
    ChaseAndAttackStrategy(int participantId);

    void onUpdate(uint32_t timeSinceLastFrame, bool& quit);
    void onNextTurn(void);
    bool endTurnCondition(void);
};