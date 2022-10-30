#pragma once

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <iostream>

#include "core/net/client/clientmessagescontroller.h"
#include "game/application/application.h"
#include "graphics/gridrenderer.h"
#include "graphics/textureloader.h"
#include "game/entities/entity.h"
#include "game/entities/entitypool.h"
#include "game/weapons/weapon.h"
#include "game/application/turncontroller.h"
#include "player.h"

class PlayerController {
private:
    std::shared_ptr<ClientMessagesController> clientMessagesController;

    std::shared_ptr<TurnController::Participant> participant;

    std::vector<std::shared_ptr<Entity>> selectedEntities;
    std::shared_ptr<GridRenderer> grid;
    std::shared_ptr<EntityPool> entityPool;

    void move(const glm::ivec2& mouseCoords);

public:
    PlayerController(
        std::shared_ptr<ClientMessagesController> clientMessagesController,
        std::shared_ptr<GridRenderer> grid, 
        std::shared_ptr<EntityPool> entityPool
    );

    void handleKeyPress(SDL_Event event);
    void handleMouseEvent(SDL_Event event);

    std::vector<std::shared_ptr<Entity>> getSelectedEntities(void);
    void setParticipant(std::shared_ptr<TurnController::Participant> participant);

    void toggleSelection(std::shared_ptr<Entity> entity);
};
