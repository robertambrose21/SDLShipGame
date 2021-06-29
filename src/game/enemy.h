#pragma once

#include "entity.h"
#include "../player/player.h"

class Enemy : public Entity {
private:
    std::shared_ptr<Player> player;

public:
    Enemy(
        std::shared_ptr<GridRenderer> grid,
        const std::string& name,
        std::shared_ptr<Player> player,
        Entity::Stats stats
    );

    void additionalUpdate(const Uint32& timeSinceLastFrame, bool& quit);
};
