#pragma once

#include "../graphics/gridrenderer.h"
#include "../game/entity.h"

class Player : public Entity {
private:

public:
    Player(std::shared_ptr<GridRenderer> grid);

    void additionalUpdate(const Uint32& timeSinceLastFrame, bool& quit);
    bool endTurnCondition(void);

    void shoot(std::shared_ptr<Entity> entity);
};
