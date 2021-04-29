#include "playercontroller.h"

const Uint32 MOVES_PER_SECOND = 5;

PlayerController::PlayerController(std::shared_ptr<Grid> grid, int movesPerTurn) :
    Entity(grid, "Player", movesPerTurn),
    partialPosition({ 0.0f, 0.0f }),
    moveVector({0, 0})
{ }

void PlayerController::handleKeyPress(SDL_Event event) {
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
    partialPosition += glm::vec2(moveVector) * (timeSinceLastFrame / (1000.0f / (MOVES_PER_SECOND * getMovesPerTurn())));
    setPosition(partialPosition);
}