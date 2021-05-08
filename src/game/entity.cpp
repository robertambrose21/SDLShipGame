#include "entity.h"

Entity::Entity(std::shared_ptr<Grid> grid, const std::string& name, Stats stats) :
    grid(grid),
    name(name),
    stats(stats),
    partialPosition({ 0.0f, 0.0f }),
    position({ 0, 0 })
{ }

void Entity::setTexture(std::shared_ptr<Texture> texture) {
    this->texture = texture;
}

void Entity::draw(std::shared_ptr<SDL_Renderer> renderer) {
    auto realPosition = grid->getTilePosition(position.x, position.y);
    SDL_Rect dst = { realPosition.x, realPosition.y, grid->getTileSize(), grid->getTileSize() };
    texture->draw(renderer, NULL, &dst);
}

Entity::Stats Entity::getStats(void) const {
    return stats;
}

const float Entity::getSpeed(void) {
    return 1000.0f / (MOVES_PER_SECOND * getStats().movesPerTurn);
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