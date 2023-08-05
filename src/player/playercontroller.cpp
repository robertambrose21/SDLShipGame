#include "playercontroller.h"

PlayerController::PlayerController(
    GameClientMessagesTransmitter& clientMessagesTransmitter,
    ApplicationContext& context
) :
    clientMessagesTransmitter(clientMessagesTransmitter),
    gridRenderer(context.getGraphicsContext().getGridRenderer()),
    turnController(context.getTurnController()),
    entityPool(context.getEntityPool()),
    camera(context.getGraphicsContext().getGridRenderer().getCamera()),
    isLeftShiftPressed(false),
    cameraVector(glm::ivec2(0, 0))
{
    dice = std::make_unique<Dice>(3, clientMessagesTransmitter);
    playerPanel = std::make_unique<PlayerPanel>(1920, 1080);
    
    turnController.subscribe(playerPanel.get());
    entityPool.subscribe(playerPanel.get());
    context.getWeaponController().subscribe(playerPanel.get());
    context.getProjectilePool().subscribe(playerPanel.get());
    context.getAreaOfEffectPool().subscribe(playerPanel.get());
}

void PlayerController::update(uint32_t timeSinceLastFrame) {
    camera.move(cameraVector * (int) timeSinceLastFrame);

    glm::ivec2 mousePosition;
    SDL_GetMouseState(&mousePosition.x, &mousePosition.y);
    auto [x, y] = gridRenderer.getTileIndices(mousePosition - camera.getPosition());
    setHoverTiles(x, y);

    for(auto entity : selectedEntities) {
        if(entity == nullptr || entity->getCurrentHP() <= 0) {
            selectedEntities.erase(std::remove(selectedEntities.begin(), selectedEntities.end(), entity), selectedEntities.end());
        }
    }
}

void PlayerController::draw(GraphicsContext& graphicsContext) {
    dice->draw(graphicsContext);
    playerPanel->draw(graphicsContext.getRenderer());

    if(selection.isActive) {
        auto size = selection.end - selection.start;

        SDL_Rect rect = { selection.start.x, selection.start.y, size.x, size.y };
        SDL_SetRenderDrawColor(graphicsContext.getRenderer(), 0x00, 0xFF, 0x00, 0xFF);
        SDL_RenderDrawRect(graphicsContext.getRenderer(), &rect);
    }

    for(auto tile : hoverTiles) {
        auto const& realPosition = gridRenderer.getTilePosition(tile.x, tile.y) + camera.getPosition();
        SDL_Rect dst = { realPosition.x, realPosition.y, gridRenderer.getTileSize(), gridRenderer.getTileSize() };
        SDL_SetRenderDrawBlendMode(graphicsContext.getRenderer(), SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(graphicsContext.getRenderer(), 0x00, 0xFF, 0x00, 0x7F);
        SDL_RenderFillRect(graphicsContext.getRenderer(), &dst);
    }
}

void PlayerController::handleKeyPress(const SDL_Event& event) {
    if(event.type == SDL_KEYDOWN && event.key.repeat == 0) {
        switch(event.key.keysym.sym) {
            case SDLK_p: {
                clientMessagesTransmitter.sendPassParticipantTurnMessage(participant->id);
                Application::getContext().getTurnController().passParticipant(participant->id);
                break;
            }

            case SDLK_d: {
                if(!participant->hasRolledForActions) {
                    dice->roll(participant->id);
                }
                break;
            }

            case SDLK_LSHIFT:
                isLeftShiftPressed = true;
                break;

            case SDLK_RIGHT:
                cameraVector += glm::ivec2(-1, 0);
                break;

            case SDLK_LEFT:
                cameraVector += glm::ivec2(1, 0);
                break;

            case SDLK_UP:
                cameraVector += glm::ivec2(0, 1);
                break;

            case SDLK_DOWN:
                cameraVector += glm::ivec2(0, -1);
                break;

            default:
                break;
        }
    }
    if(event.type == SDL_KEYUP) {
        switch(event.key.keysym.sym) {
            case SDLK_LSHIFT:
                isLeftShiftPressed = false;
                break;

            case SDLK_RIGHT:
                cameraVector -= glm::ivec2(-1, 0);
                break;

            case SDLK_LEFT:
                cameraVector -= glm::ivec2(1, 0);
                break;

            case SDLK_UP:
                cameraVector -= glm::ivec2(0, 1);
                break;

            case SDLK_DOWN:
                cameraVector -= glm::ivec2(0, -1);
                break;

            default:
                break;
        }
    }
}

void PlayerController::handleMouseEvent(const SDL_Event& event) {
    auto mousePosition = glm::ivec2(event.button.x, event.button.y);
    auto [x, y] = gridRenderer.getTileIndices(mousePosition - camera.getPosition());

    switch(event.type) {
        case SDL_MOUSEBUTTONDOWN: {
            handleMouseDown(event);
            break;
        }
        
        case SDL_MOUSEBUTTONUP: {
            handleMouseUp(event);
            break;
        }

        case SDL_MOUSEMOTION: {
            if(selection.isActive) {
                selection.end = glm::ivec2(event.motion.x, event.motion.y);
            }

            break;
        }

        default:
            break;
    }
}

void PlayerController::handleMouseDown(const SDL_Event& event) {
    auto mousePosition = glm::ivec2(event.button.x, event.button.y);
    auto [x, y] = gridRenderer.getTileIndices(mousePosition - camera.getPosition());
    auto position = glm::ivec2(x, y);

    switch(event.button.button) {
        case SDL_BUTTON_LEFT: {
            if(isLeftShiftPressed) {
                attack(position);
                break;
            }

            auto entity = Entity::filterByTile(x, y, participant->entities);

            if(entity != nullptr) {
                toggleSelection({ entity });
            }
            else {
                selection.isActive = true;
                selection.start = mousePosition;
                selection.end = mousePosition;

                deselectAll();
            }
            break;
        }

        case SDL_BUTTON_RIGHT: {
            auto const& target = Entity::filterByTile(x, y, entityPool.getEntities());

            if(target != nullptr || isLeftShiftPressed) {
                attack(position);
            }
            else {
                move(glm::ivec2(x, y));
            }
            
            break;
        }
    }
}

void PlayerController::handleMouseUp(const SDL_Event& event) {
    auto& grid = gridRenderer.getGrid();

    switch(event.button.button) {
        case SDL_BUTTON_LEFT: {
            if(selection.isActive) {
                selection.isActive = false;

                auto start = glm::ivec2(
                    std::min(selection.start.x, selection.end.x),
                    std::min(selection.start.y, selection.end.y)
                ) - camera.getPosition();
                auto end = glm::ivec2(
                    std::max(selection.start.x, selection.end.x),
                    std::max(selection.start.y, selection.end.y)
                ) - camera.getPosition();

                auto [x, y] = gridRenderer.getTileIndices(start);
                auto [sizeX, sizeY] = gridRenderer.getTileIndices(end - start);

                auto tiles = gridRenderer.getGrid().getTilesInSquare(x, y, sizeX, sizeY);
                
                toggleSelection(Entity::filterByTiles(tiles, participant->entities));
            }
            break;
        }
    }
}

void PlayerController::toggleSelection(const std::vector<Entity*>& entities) {
    if(entities.empty()) {
        return;
    }

    if(entities.size() == 1 && entities[0] != nullptr && entities[0]->isSelected()) {
        selectedEntities.erase(std::find(selectedEntities.begin(), selectedEntities.end(), entities[0]));
        entities[0]->setSelected(false);
        return;
    }

    for(auto entity : entities) {
        clientMessagesTransmitter.sendSelectEntityMessage(entity->getId());
        selectedEntities.push_back(entity);
        entity->setSelected(true);
    }
}

void PlayerController::deselectAll(void) {
    for(auto entity : selectedEntities) {
        entity->setSelected(false);
    }

    selectedEntities.clear();
}

void PlayerController::move(const glm::ivec2& position) {
    for(auto const& entity : selectedEntities) {
        clientMessagesTransmitter.sendFindPathMessage(entity->getId(), position, 0);
        if(turnController.performMoveAction(entity, position)) {
            dice->clickAction(0);
        }
    }
}

void PlayerController::attack(const glm::ivec2& target) {
    for(auto const& entity : selectedEntities) {
        auto const& weapon = entity->getCurrentWeapon();

        clientMessagesTransmitter.sendAttackMessage(
            entity->getId(), 
            target, 
            weapon->getId()
        );
        
        if(turnController.performAttackAction(entity, weapon, target)) {
            dice->clickAction(1);
        }
    }
}

void PlayerController::setHoverTiles(int centerTileX, int centerTileY) {
    if(!isLeftShiftPressed || selectedEntities.empty()) {
        hoverTiles.clear();
        return;
    }

    auto entity = selectedEntities[0];
    auto weapon = entity->getCurrentWeapon();
    
    if(weapon->getName() == "Grenade Launcher") {
        hoverTiles = gridRenderer.getGrid().getTilesInCircle(centerTileX, centerTileY, 2);
    }
    else if(weapon->getName()== "Freeze Gun") {
        auto& grid = gridRenderer.getGrid();
        auto target = gridRenderer.getTilePosition(centerTileX, centerTileY);

        glm::ivec2 dir = entity->getPosition() - glm::ivec2(centerTileX, centerTileY);
        auto perp = glm::normalize(glm::vec2(dir.y, -dir.x));
        auto pX = std::min(grid.getWidth() - 1, (int) std::round(perp.x));
        auto pY = std::min(grid.getHeight() - 1, (int) std::round(perp.y));

        hoverTiles = {
            glm::ivec2(centerTileX, centerTileY),
            glm::ivec2(centerTileX + pX, centerTileY + pY),
            glm::ivec2(centerTileX - pX, centerTileY - pY)
        };
    }
}

const std::vector<Entity*>& PlayerController::getSelectedEntities(void) const {
    return selectedEntities;
}

void PlayerController::setParticipant(TurnController::Participant* participant) {
    game_assert(participant != nullptr);
    game_assert(participant->isPlayer);
    this->participant = participant;
}

Dice& PlayerController::getDice(void) {
    return *dice;
}
