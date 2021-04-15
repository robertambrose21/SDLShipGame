#include "playercontroller.h"

PlayerController::PlayerController(std::shared_ptr<Grid> grid) :
    grid(grid),
    position({0, 0}),
    moveVector({0, 0})
{ }

void PlayerController::setTexture(std::shared_ptr<Texture> texture) {
    this->texture = texture;
}

void PlayerController::draw(std::shared_ptr<SDL_Renderer> renderer) {
    SDL_Rect realPosition = grid->getTilePosition(position.x, position.y);
    texture->draw(renderer, NULL, &realPosition);
}

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

void PlayerController::move(const Uint32& timeSinceLastFrame) {
    position += moveVector * (int) timeSinceLastFrame;
}