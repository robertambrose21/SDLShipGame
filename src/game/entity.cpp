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

    additionalUpdate(timeSinceLastFrame, quit);

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

bool Entity::findPath(const glm::ivec2& target, const int& stopShortSteps) {
    auto path = grid->getGrid()->findPath(getPosition(), target);

    if(path.empty()) {
        return false;
    }

    // Remove the initial path node which is just the entities current position
    path.pop_front(); 

    if(path.size() >= stopShortSteps) {
        for(auto i = 0; i < stopShortSteps; i++) {
            path.pop_back();
        }
    }

    this->path = path;
    return !path.empty();
}

bool Entity::isNeighbour(std::shared_ptr<Entity> entity) const {
    return glm::distance(glm::vec2(getPosition()), glm::vec2(entity->getPosition())) < 2;
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
