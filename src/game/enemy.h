#pragma once

#include "entity.h"
#include "../player/playercontroller.h"

class Enemy : public Entity {
private:
    std::shared_ptr<PlayerController> player;

public:
    Enemy(
        std::shared_ptr<GridRenderer> grid,
        const std::string& name,
        std::shared_ptr<PlayerController> player,
        Entity::Stats stats
    );

    void update(const Uint32& timeSinceLastFrame, bool& quit);
};
