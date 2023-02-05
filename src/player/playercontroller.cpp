#include "playercontroller.h"

PlayerController::PlayerController(
    const std::shared_ptr<GameClientMessagesTransmitter>& clientMessagesTransmitter,
    const std::shared_ptr<ApplicationContext>& context
) :
    clientMessagesTransmitter(clientMessagesTransmitter),
    grid(context->getGraphicsContext()->getGridRenderer()),
    entityPool(context->getEntityPool()),
    turnController(context->getTurnController())
{
    dice = std::make_shared<Dice>(3, clientMessagesTransmitter);
}

void PlayerController::update(uint32_t timeSinceLastFrame) {
    for(auto entity : selectedEntities) {
        if(entity == nullptr || entity->getCurrentHP() <= 0) {
            selectedEntities.erase(std::remove(selectedEntities.begin(), selectedEntities.end(), entity), selectedEntities.end());
        }
    }
}

void PlayerController::draw(const std::shared_ptr<GraphicsContext>& graphicsContext) {
    dice->draw(graphicsContext);
}

void PlayerController::handleKeyPress(const SDL_Event& event) {
    if(event.type == SDL_KEYDOWN) {
        switch(event.key.keysym.sym) {
            case SDLK_p: {
                clientMessagesTransmitter->sendPassParticipantTurnMessage(participant->id);
                Application::getContext()->getTurnController()->passParticipant(participant->id);
                break;
            }

            case SDLK_d: {
                if(!participant->hasRolledForActions) {
                    dice->roll(participant->id);
                }
                break;
            }

            default:
                break;
        }
    }
}

void PlayerController::handleMouseEvent(const SDL_Event& event) {
    if(event.type == SDL_MOUSEBUTTONDOWN) {
        int x, y;
        SDL_GetMouseState(&x, &y);

        auto [dX, dY] = grid->getTileIndices(glm::ivec2(x, y));

        switch(event.button.button) {
            case SDL_BUTTON_LEFT: {
                auto const& entity = Entity::filterByTile(dX, dY, participant->entities);

                toggleSelection(entity);

                dice->handleClickEvent(x, y);
                break;
            }
        

            case SDL_BUTTON_RIGHT: {
                auto const& target = Entity::filterByTile(dX, dY, entityPool->getEntities());

                if(target != nullptr) {
                    for(auto const& entity : selectedEntities) {
                        auto const& weapon = entity->getCurrentWeapon();

                        clientMessagesTransmitter->sendAttackEntityMessage(
                            entity->getId(), 
                            target->getId(), 
                            weapon->getId()
                        );
                        
                        if(turnController->performAttackAction(entity, weapon, target)) {
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

void PlayerController::toggleSelection(const std::shared_ptr<Entity>& entity) {
    if(entity == nullptr) {
        return;
    }

    clientMessagesTransmitter->sendSelectEntityMessage(entity->getId());

    if(entity->isSelected()) {
        selectedEntities.erase(std::find(selectedEntities.begin(), selectedEntities.end(), entity));
        entity->setSelected(false);
    }
    else {
        selectedEntities.push_back(entity);
        entity->setSelected(true);
    }
}

void PlayerController::move(const glm::ivec2& mouseCoords) {
    auto [dX, dY] = grid->getTileIndices(mouseCoords);

    for(auto const& entity : selectedEntities) {
        clientMessagesTransmitter->sendFindPathMessage(entity->getId(), {dX, dY}, 0);
        if(turnController->performMoveAction(entity, glm::ivec2(dX, dY))) {
            dice->removeAction(0);
        }
    }
}

const std::vector<std::shared_ptr<Entity>>& PlayerController::getSelectedEntities(void) const {
    return selectedEntities;
}

void PlayerController::setParticipant(const std::shared_ptr<TurnController::Participant>& participant) {
    game_assert(participant != nullptr);
    game_assert(participant->isPlayer);
    this->participant = participant;
}

std::shared_ptr<Dice> PlayerController::getDice(void) {
    return dice;
}
