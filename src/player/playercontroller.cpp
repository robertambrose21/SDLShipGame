#include "playercontroller.h"

PlayerController::PlayerController(std::shared_ptr<GridRenderer> grid) :
    Entity(grid, "Player", { 2, 10 }),
    destination({ 0, 0}),
    direction({ 0.0f, 0.0f })
{ }

void PlayerController::handleKeyPress(SDL_Event event) {
    //
}

void PlayerController::handleMouseEvent(SDL_Event event) {
    if(event.type == SDL_MOUSEBUTTONDOWN) {
        int x, y;
        SDL_GetMouseState(&x, &y);

        // TODO: A* algorithm
        destination = glm::ivec2(x, y) / grid->getTileSize();
        direction = glm::normalize(glm::vec2(destination) - getPartialPosition());
    }
}

void PlayerController::update(const Uint32& timeSinceLastFrame, bool& quit) {
    if(getPosition() == destination) {
        return;
    }

    setPartialPosition(getPartialPosition() + (direction * (timeSinceLastFrame / getSpeed())));
}