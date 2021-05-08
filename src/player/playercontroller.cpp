#include "playercontroller.h"

PlayerController::PlayerController(std::shared_ptr<GridRenderer> grid) :
    grid(grid)
{
    player = std::make_shared<Player>(grid);
}

void PlayerController::handleKeyPress(SDL_Event event) {
    //
}

void PlayerController::handleMouseEvent(SDL_Event event) {
    if(event.type == SDL_MOUSEBUTTONDOWN) {
        int x, y;
        SDL_GetMouseState(&x, &y);
        player->setDestination(glm::ivec2(x, y) / grid->getTileSize());
    }
}

std::shared_ptr<Player> PlayerController::getEntity(void) {
    return player;
}
