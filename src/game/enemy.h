#pragma once

#include "entity.h"
#include "../player/player.h"

class Enemy : public Entity {
private:
    std::shared_ptr<Player> player;
    std::shared_ptr<Weapon> teeth;

public:
    Enemy(
        std::shared_ptr<GridRenderer> grid,
        const std::string& name,
        std::shared_ptr<Player> player,
        Entity::Stats stats,
        std::vector<std::shared_ptr<Weapon>> weapons
    );

    void additionalUpdate(const Uint32& timeSinceLastFrame, bool& quit);
    bool endTurnCondition(void);
    void bite(void);
};
