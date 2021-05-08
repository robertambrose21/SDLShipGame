#pragma once

#include "../graphics/gridrenderer.h"
#include "../game/entity.h"

class Player : public Entity {
private:
    glm::ivec2 destination;
    glm::vec2 direction;

public:
    Player(std::shared_ptr<GridRenderer> grid);

    void update(const Uint32& timeSinceLastFrame, bool& quit);

    void setDestination(const glm::ivec2& destination);
};
