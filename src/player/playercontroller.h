#pragma once

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <iostream>
#include <random>

#include "game/net/client/gameclientmessagestransmitter.h"
#include "game/application/application.h"
#include "graphics/gridrenderer.h"
#include "graphics/textureloader.h"
#include "game/entities/entity.h"
#include "game/entities/entitypool.h"
#include "game/weapons/weapon.h"
#include "game/application/turncontroller.h"
#include "graphics/dice.h"
#include "graphics/button.h"

class PlayerController {
private:
    std::shared_ptr<GameClientMessagesTransmitter> clientMessagesTransmitter;

    std::shared_ptr<TurnController::Participant> participant;

    std::vector<std::shared_ptr<Entity>> selectedEntities;
    std::shared_ptr<GridRenderer> grid;
    std::shared_ptr<EntityPool> entityPool;
    std::shared_ptr<TurnController> turnController;

    std::shared_ptr<Dice> dice;

    void move(const glm::ivec2& mouseCoords);

public:
    PlayerController(
        const std::shared_ptr<GameClientMessagesTransmitter>& clientMessagesTransmitter,
        const std::shared_ptr<ApplicationContext>& context
    );

    void update(uint32_t timeSinceLastFrame);
    void draw(const std::shared_ptr<GraphicsContext>& graphicsContext);

    void handleKeyPress(const SDL_Event& event);
    void handleMouseEvent(const SDL_Event& event);

    const std::vector<std::shared_ptr<Entity>>& getSelectedEntities(void) const;
    void setParticipant(const std::shared_ptr<TurnController::Participant>& participant);

    void toggleSelection(const std::shared_ptr<Entity>& entity);

    std::shared_ptr<Dice> getDice(void);
};
