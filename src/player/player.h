#pragma once

#include "../graphics/gridrenderer.h"
#include "../game/entity.h"

class Player : public Entity {
private:
    std::deque<glm::ivec2> path;
    Uint32 timeSinceLastMoved;

public:
    Player(std::shared_ptr<GridRenderer> grid);

    void update(const Uint32& timeSinceLastFrame, bool& quit);
    void setPath(std::deque<glm::ivec2> path);
};
