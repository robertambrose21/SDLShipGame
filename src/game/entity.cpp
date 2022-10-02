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
    movesLeft(0),
    timeSinceLastMoved(0)
{ }

void Entity::setTexture(std::shared_ptr<Texture> texture) {
    this->texture = texture;
}

void Entity::draw(std::shared_ptr<SDL_Renderer> renderer) {
    auto realPosition = grid->getTilePosition(position.x, position.y);
    SDL_Rect dst = { realPosition.x, realPosition.y, grid->getTileSize(), grid->getTileSize() };
    texture->draw(renderer, NULL, &dst);

    for(auto weapon : weapons) {
        weapon->draw(renderer);
    }
}

void Entity::update(const Uint32& timeSinceLastFrame, bool& quit) {
    for(auto weapon : weapons) {
        weapon->update(timeSinceLastFrame);
    }

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

void Entity::takeDamage(const int& amount) {
    stats.hp -= amount;
}

void Entity::attack(std::shared_ptr<Entity> target, std::shared_ptr<Weapon> weapon) {
    auto targetName = target->getName();

    weapon->use(position, target);

    std::cout
        << "["
        << name
        << "] attacked the [" 
        << targetName
        << "] for [" 
        << weapon->getStats().damage 
        << "] damage with ["
        << weapon->getName()
        << "], ["
        << targetName
        << "] now has [" 
        << (target == nullptr ? 0 : target->getStats().hp - weapon->getStats().damage)
        << "] hp" 
        << std::endl;
}

std::vector<std::shared_ptr<Weapon>> Entity::getWeapons(void) const {
    return weapons;
}

std::shared_ptr<Weapon> Entity::addWeapon(std::shared_ptr<Weapon> weapon) {
    weapons.push_back(weapon);
    return weapon;
}

void Entity::removeWeapon(const std::string& name) {
    for(auto i = 0; i < weapons.size(); i++) {
        if(weapons[i]->getName() == name) {
            weapons.erase(weapons.begin() + i);
        }
    }
}

void Entity::setCurrentWeapon(std::shared_ptr<Weapon> weapon) {
    currentWeapon = weapon;
}

std::shared_ptr<Weapon> Entity::getCurrentWeapon(void) {
    return currentWeapon;
}

std::string Entity::getName(void) const {
    return name;
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

bool Entity::isTurnInProgress(void) const {
    return getMovesLeft() > 0 && !currentWeapon->hasFinished();
}

void Entity::useMoves(const int& numMoves) {
    movesLeft -= numMoves;
    
    if(movesLeft < 0) {
        movesLeft = 0;
    }
}

void Entity::nextTurn(void) {
    movesLeft = stats.movesPerTurn;
    
    for(auto weapon : weapons) {
        weapon->reset();
    }
}
