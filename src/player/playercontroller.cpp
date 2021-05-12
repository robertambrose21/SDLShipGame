#include "playercontroller.h"

PlayerController::PlayerController(std::shared_ptr<GridRenderer> grid, std::shared_ptr<EntityPool> entityPool) :
    grid(grid),
    entityPool(entityPool)
{
    player = std::dynamic_pointer_cast<Player>(entityPool->createEntity(std::make_shared<Player>(grid)));
}

void PlayerController::handleKeyPress(SDL_Event event) {
    //
}

void PlayerController::handleMouseEvent(SDL_Event event) {
    if(event.type == SDL_MOUSEBUTTONDOWN) {
        int x, y;
        SDL_GetMouseState(&x, &y);

        switch(event.button.button) {
        case SDL_BUTTON_LEFT:
            move(glm::ivec2(x, y));
            break;

        case SDL_BUTTON_RIGHT:
            for(auto entity : entityPool->getEntities()) {
                if(entity != player) {
                    entity->doDamage(1);
                }
            }
            break;
        }
    }
}

void PlayerController::move(const glm::ivec2& mouseCoords) {
    auto [dX, dY] = grid->getTileIndices(mouseCoords);
    player->setPath(grid->getGrid()->findPath(player->getPosition(), glm::ivec2(dX, dY)));
}

std::shared_ptr<Player> PlayerController::getEntity(void) {
    return player;
}
