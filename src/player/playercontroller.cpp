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

    for(auto entity : selectedEntities) {
        if(entity == nullptr || entity->getCurrentHP() <= 0) {
            selectedEntities.erase(std::remove(selectedEntities.begin(), selectedEntities.end(), entity), selectedEntities.end());
        }
    }
}

void PlayerController::draw(GraphicsContext& graphicsContext) {
    dice->draw(graphicsContext);
    playerPanel->draw(graphicsContext.getRenderer());
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
    if(event.type == SDL_MOUSEBUTTONDOWN) {
        auto [x, y] = gridRenderer.getTileIndices(glm::ivec2(event.button.x, event.button.y) - camera.getPosition());

        switch(event.button.button) {
            case SDL_BUTTON_LEFT: {
                auto const& entity = Entity::filterByTile(x, y, participant->entities);

                toggleSelection(entity);

                dice->handleClickEvent(x, y);
                break;
            }
        

            case SDL_BUTTON_RIGHT: {
                auto const& target = Entity::filterByTile(x, y, entityPool.getEntities());
                auto position = glm::ivec2(x, y);

                if(target != nullptr || isLeftShiftPressed) {
                    for(auto const& entity : selectedEntities) {
                        auto const& weapon = entity->getCurrentWeapon();

                        clientMessagesTransmitter.sendAttackMessage(
                            entity->getId(), 
                            position, 
                            weapon->getId()
                        );
                        
                        if(turnController.performAttackAction(entity, weapon, position)) {
                            // TODO: Bug
                            dice->removeAction(1);
                        }
                    }
                }
                else {
                    move(glm::ivec2(x, y));
                }
                
                break;
            }
        }
    }
}

void PlayerController::toggleSelection(Entity* entity) {
    if(entity == nullptr) {
        return;
    }

    clientMessagesTransmitter.sendSelectEntityMessage(entity->getId());

    if(entity->isSelected()) {
        selectedEntities.erase(std::find(selectedEntities.begin(), selectedEntities.end(), entity));
        entity->setSelected(false);
    }
    else {
        selectedEntities.push_back(entity);
        entity->setSelected(true);
    }
}

void PlayerController::move(const glm::ivec2& position) {
    for(auto const& entity : selectedEntities) {
        clientMessagesTransmitter.sendFindPathMessage(entity->getId(), position, 0);
        if(turnController.performMoveAction(entity, position)) {
            dice->removeAction(0);
        }
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
