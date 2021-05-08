#pragma once

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <iostream>

#include "../graphics/gridrenderer.h"
#include "../graphics/textureloader.h"
#include "../game/entity.h"
#include "player.h"

class PlayerController {
private:
    std::shared_ptr<Player> player;
    std::shared_ptr<GridRenderer> grid;

public:
    PlayerController(std::shared_ptr<GridRenderer> grid);

    void handleKeyPress(SDL_Event event);
    void handleMouseEvent(SDL_Event event);

    std::shared_ptr<Player> getEntity(void);
};
