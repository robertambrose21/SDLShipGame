#include "entity.h"

Entity::Entity(
    std::shared_ptr<GridRenderer> grid,
    const std::string& name,
    Stats stats
) :
    grid(grid),
    name(name),
    stats(stats),
    position({ 0, 0 }),
    movesLeft(0)
{ }

void Entity::setTexture(std::shared_ptr<Texture> texture) {
    this->texture = texture;
}

void Entity::draw(std::shared_ptr<SDL_Renderer> renderer) {
    auto realPosition = grid->getTilePosition(position.x, position.y);
    SDL_Rect dst = { realPosition.x, realPosition.y, grid->getTileSize(), grid->getTileSize() };
    texture->draw(renderer, NULL, &dst);
}

void Entity::update(const Uint32& timeSinceLastFrame, bool& quit) {
    if(getMovesLeft() == 0) {
        return;
    }

    if(path.empty()) {
        return;
    }

    timeSinceLastMoved += timeSinceLastFrame;

    if(timeSinceLastMoved > getSpeed()) {
        setPosition(path.front());
        path.pop_front();
        timeSinceLastMoved = 0;
        useMoves(1);
    }

    additionalUpdate(timeSinceLastFrame, quit);
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

void Entity::setPath(std::deque<glm::ivec2> path) {
    this->path = path;
}

int Entity::getMovesLeft(void) const {
    return movesLeft;
}

void Entity::useMoves(const int& numMoves) {
    movesLeft -= numMoves;
    
    if(movesLeft < 0) {
        movesLeft = 0;
    }
}

void Entity::nextTurn(void) {
    movesLeft = stats.movesPerTurn;
}
