#include "playercontroller.h"

PlayerController::PlayerController(std::shared_ptr<GridRenderer> grid, std::shared_ptr<EntityPool> entityPool) :
    grid(grid),
    entityPool(entityPool)
{
    player = std::make_shared<Player>(grid);
}

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

        switch(event.button.button) {
        case SDL_BUTTON_LEFT:
            move(glm::ivec2(x, y));
            break;

        case SDL_BUTTON_RIGHT:
            auto [dX, dY] = grid->getTileIndices(glm::ivec2(x, y));

            for(auto entity : entityPool->getEntitiesOnTile(dX, dY)) {
                if(entity != player) {
                    player->attack(entity, player->getCurrentWeapon());
                }
            }
            break;
        }
    }
}

void PlayerController::move(const glm::ivec2& mouseCoords) {
    auto [dX, dY] = grid->getTileIndices(mouseCoords);
    player->findPath(glm::ivec2(dX, dY));
}

std::shared_ptr<Player> PlayerController::getEntity(void) {
    return player;
}
