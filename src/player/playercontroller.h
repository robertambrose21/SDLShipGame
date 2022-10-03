#pragma once

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <iostream>

#include "../game/application.h"
#include "../graphics/gridrenderer.h"
#include "../graphics/textureloader.h"
#include "../game/entity.h"
#include "../game/entitypool.h"
#include "../game/weapon.h"
#include "player.h"

class PlayerController {
private:
    std::shared_ptr<Player> player;
    std::shared_ptr<GridRenderer> grid;
    std::shared_ptr<EntityPool> entityPool;

    void move(const glm::ivec2& mouseCoords);

public:
    PlayerController(std::shared_ptr<GridRenderer> grid, std::shared_ptr<EntityPool> entityPool);

    void handleKeyPress(SDL_Event event);
    void handleMouseEvent(SDL_Event event);

    std::shared_ptr<Player> getEntity(void);
};
