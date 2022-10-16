#pragma once

#include "../graphics/gridrenderer.h"
#include "../game/entity.h"

class Player : public Entity {
private:

public:
    Player();

    void additionalUpdate(const Uint32& timeSinceLastFrame, bool& quit);
    bool endTurnCondition(void);
};
