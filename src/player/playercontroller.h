#pragma once

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <iostream>

#include "../graphics/gridrenderer.h"
#include "../graphics/textureloader.h"
#include "../game/entity.h"

// TODO: Split player entity and player controller
class PlayerController : public Entity {
private:
    glm::ivec2 destination;
    glm::vec2 direction;

public:
    PlayerController(std::shared_ptr<GridRenderer> grid);

    void handleKeyPress(SDL_Event event);
    void handleMouseEvent(SDL_Event event);

    void update(const Uint32& timeSinceLastFrame, bool& quit);
};
