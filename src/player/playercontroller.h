#pragma once

#include <SDL2/SDL.h>
#include <glm/glm.hpp>

#include "../graphics/grid.h"
#include "../graphics/textureloader.h"
#include "../game/entity.h"

class PlayerController : public Entity {
private:
    glm::ivec2 moveVector;

    void handleMovement(SDL_Event event);

public:
    PlayerController(std::shared_ptr<Grid> grid);

    void handleKeyPress(SDL_Event event);
    void update(const Uint32& timeSinceLastFrame, bool& quit);
};
