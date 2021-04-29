#pragma once

#include <SDL2/SDL.h>
#include <glm/glm.hpp>

#include "../graphics/grid.h"
#include "../graphics/textureloader.h"
#include "../game/entity.h"

class PlayerController : public Entity {
private:
    glm::ivec2 moveVector;
    glm::vec2 partialPosition;

public:
    PlayerController(std::shared_ptr<Grid> grid, int movesPerTurn = 1);

    void handleKeyPress(SDL_Event event);
    void update(const Uint32& timeSinceLastFrame, bool& quit);
};
