#include "playercontroller.h"

PlayerController::PlayerController(std::shared_ptr<Grid> grid) :
    grid(grid)
{
    position = { 0, 0 };
}

void PlayerController::setTexture(std::shared_ptr<Texture> texture) {
    this->texture = texture;
}

void PlayerController::draw(std::shared_ptr<SDL_Renderer> renderer) {
    SDL_Rect realPosition = grid->getTilePosition(position.x, position.y);
    texture->draw(renderer, NULL, &realPosition);
}

// TODO: Turn on rather than running off key repeats
void PlayerController::handleKeyPress(SDL_Event event) {
    if(event.type == SDL_KEYDOWN) {
        switch(event.key.keysym.sym) {
            case SDLK_w:
                position.y = std::max(0, position.y - 1);
                break;

            case SDLK_s:
                position.y = std::min(grid->getHeight() - 1, position.y + 1);
                break;

            case SDLK_a:
                position.x = std::max(0, position.x - 1);
                break;

            case SDLK_d:
                position.x = std::min(grid->getWidth() - 1, position.x + 1);
                break;
        }
    }
}