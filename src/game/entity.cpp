#include "entity.h"

Entity::Entity(std::shared_ptr<GridRenderer> grid, const std::string& name, Stats stats) :
    grid(grid),
    name(name),
    stats(stats),
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

void Entity::doDamage(const int& amount) {
    stats.hp -= amount;
}

glm::ivec2 Entity::getPosition(void) const {
    return position;
}

void Entity::setPosition(const glm::ivec2& position) {
    this->position = position;
}
