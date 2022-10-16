#pragma once

#include <limits>

#include "entity.h"
#include "../player/player.h"
#include "meleeweapon.h"
#include "entitypool.h"

class Enemy : public Entity {
private:
    std::shared_ptr<EntityPool> entityPool;
    std::shared_ptr<Entity> target;

    bool canPassTurn;

public:
    Enemy(
        const std::string& name,
        Entity::Stats stats
    );

    void additionalUpdate(const Uint32& timeSinceLastFrame, bool& quit);
    bool endTurnCondition(void);

    std::shared_ptr<Entity> findClosestTarget(void);

    void nextTurn();
};
