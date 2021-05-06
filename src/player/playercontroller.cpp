#include "playercontroller.h"

PlayerController::PlayerController(std::shared_ptr<Grid> grid) :
    Entity(grid, "Player", { 2, 10 }),
    moveVector({ 0, 0 })
{ }

void PlayerController::handleKeyPress(SDL_Event event) {
    handleMovement(event);
}

void PlayerController::handleMovement(SDL_Event event) {
    if(event.type == SDL_KEYDOWN && event.key.repeat == 0) {
        switch(event.key.keysym.sym) {
            case SDLK_w:
                moveVector.y += -1;
                break;

            case SDLK_s:
                moveVector.y += 1;
                break;

            case SDLK_a:
                moveVector.x += -1;
                break;

            case SDLK_d:
                moveVector.x += 1;
                break;
        }
    }
    else if(event.type == SDL_KEYUP && event.key.repeat == 0) {
        switch(event.key.keysym.sym) {
            case SDLK_w:
                moveVector.y -= -1;
                break;

            case SDLK_s:
                moveVector.y -= 1;
                break;

            case SDLK_a:
                moveVector.x -= -1;
                break;

            case SDLK_d:
                moveVector.x -= 1;
                break;
        }
    }
}

void PlayerController::update(const Uint32& timeSinceLastFrame, bool& quit) {
    setPartialPosition(getPartialPosition() + glm::vec2(moveVector) * (timeSinceLastFrame / getSpeed()));
}