#pragma once

#include "entity.h"
#include "../player/player.h"
#include "meleeweapon.h"
#include "application.h"

class Enemy : public Entity {
private:
    std::shared_ptr<Player> player;
    bool canPassTurn;

public:
    Enemy(
        std::shared_ptr<GridRenderer> grid,
        const std::string& name,
        std::shared_ptr<Player> player,
        Entity::Stats stats
    );

    void additionalUpdate(const Uint32& timeSinceLastFrame, bool& quit);
    bool endTurnCondition(void);

    void nextTurn();
};
