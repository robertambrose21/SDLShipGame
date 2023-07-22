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
#include "graphics/ui/dice.h"
#include "graphics/ui/button.h"
#include "graphics/ui/playerpanel.h"
#include "graphics/ui/textpanel.h"

class PlayerController {
private:
    GameClientMessagesTransmitter& clientMessagesTransmitter;

    TurnController::Participant* participant;

    std::vector<Entity*> selectedEntities;
    GridRenderer& gridRenderer;
    TurnController& turnController;
    EntityPool& entityPool;

    std::unique_ptr<Dice> dice;
    bool isLeftShiftPressed;

    Camera& camera;
    glm::ivec2 cameraVector;

    std::unique_ptr<PlayerPanel> playerPanel;

    void move(const glm::ivec2& position);

public:
    PlayerController(
        GameClientMessagesTransmitter& clientMessagesTransmitter,
        ApplicationContext& context
    );

    void update(uint32_t timeSinceLastFrame);
    void draw(GraphicsContext& graphicsContext);

    void handleKeyPress(const SDL_Event& event);
    void handleMouseEvent(const SDL_Event& event);

    const std::vector<Entity*>& getSelectedEntities(void) const;
    void setParticipant(TurnController::Participant* participant);

    void toggleSelection(Entity* entity);

    Dice& getDice(void);
};
