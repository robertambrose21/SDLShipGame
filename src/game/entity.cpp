#include "entity.h"

const Uint32 MOVES_PER_SECOND = 5;

Entity::Entity(std::shared_ptr<Grid> grid, const std::string& name, int movesPerTurn) :
    grid(grid),
    name(name),
    movesPerTurn(movesPerTurn),
    position({0, 0})
{ }

void Entity::setTexture(std::shared_ptr<Texture> texture) {
    this->texture = texture;
}

void Entity::draw(std::shared_ptr<SDL_Renderer> renderer) {
    SDL_Rect realPosition = grid->getTilePosition(position.x, position.y);
    texture->draw(renderer, NULL, &realPosition);
}

int Entity::getMovesPerTurn(void) const {
    return movesPerTurn;
}

glm::ivec2 Entity::getPosition(void) const {
    return position;
}

void Entity::setPosition(const glm::ivec2& position) {
    this->position = position;
}