#include "entity.h"

Entity::Entity(std::shared_ptr<Grid> grid, const std::string& name, int movesPerTurn) :
    grid(grid),
    name(name),
    movesPerTurn(movesPerTurn),
    partialPosition({ 0.0f, 0.0f }),
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

const float Entity::getSpeed(void) {
    return 1000.0f / (MOVES_PER_SECOND * getMovesPerTurn());
}

glm::ivec2 Entity::getPosition(void) const {
    return position;
}

void Entity::setPosition(const glm::ivec2& position) {
    this->partialPosition = position;
    this->position = position;
}

glm::vec2 Entity::getPartialPosition(void) const {
    return partialPosition;
}

void Entity::setPartialPosition(const glm::vec2& partialPosition) {
    this->partialPosition = partialPosition;
    this->position = partialPosition;
}