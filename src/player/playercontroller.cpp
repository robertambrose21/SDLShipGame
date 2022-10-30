#include "playercontroller.h"

PlayerController::PlayerController(
    std::shared_ptr<ClientMessagesController> clientMessagesController,
    std::shared_ptr<GridRenderer> grid, 
    std::shared_ptr<EntityPool> entityPool
) :
    clientMessagesController(clientMessagesController),
    grid(grid),
    entityPool(entityPool)
{ }

void PlayerController::handleKeyPress(SDL_Event event) {
    if(event.type == SDL_KEYDOWN) {
        switch(event.key.keysym.sym) {
            case SDLK_p:
                Application::getContext()->getTurnController()->passCurrentParticipant();
                break;

            default:
                break;
        }
    }
}

void PlayerController::handleMouseEvent(SDL_Event event) {
    if(event.type == SDL_MOUSEBUTTONDOWN) {
        int x, y;
        SDL_GetMouseState(&x, &y);

        auto [dX, dY] = grid->getTileIndices(glm::ivec2(x, y));

        switch(event.button.button) {
            case SDL_BUTTON_LEFT: {
                auto entity = Entity::filterByTile(dX, dY, participant->entities);

                toggleSelection(entity);
                break;
            }
        

            case SDL_BUTTON_RIGHT: {
                auto target = Entity::filterByTile(dX, dY, entityPool->getEntities());

                if(target != nullptr) {
                    for(auto entity : selectedEntities) {
                        entity->attack(target, entity->getCurrentWeapon());
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

void PlayerController::toggleSelection(std::shared_ptr<Entity> entity) {
    if(entity == nullptr) {
        return;
    }

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
    
    clientMessagesController->sendFindPathMessage({dX, dY});

    for(auto entity : selectedEntities) {
        entity->findPath(glm::ivec2(dX, dY));
    }
}

std::vector<std::shared_ptr<Entity>> PlayerController::getSelectedEntities(void) {
    return selectedEntities;
}

void PlayerController::setParticipant(std::shared_ptr<TurnController::Participant> participant) {
    this->participant = participant;
}
