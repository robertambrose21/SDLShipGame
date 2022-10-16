#include "entity.h"
#include "application.h"

Entity::Entity(
    const std::string& name,
    Stats stats
) :
    name(name),
    stats(stats),
    position({ 0, 0 }),
    movesLeft(0),
    currentHP(stats.hp),
    timeSinceLastMoved(0),
    selected(false)
{
    grid = Application::getContext()->getWindow()->getGridRenderer();
}

void Entity::setTexture(std::shared_ptr<Texture> texture) {
    this->texture = texture;
}

void Entity::setSelectedTexture(std::shared_ptr<Texture> selectedTexture) {
    this->selectedTexture = selectedTexture;
}

void Entity::draw(std::shared_ptr<SDL_Renderer> renderer) {
    auto realPosition = grid->getTilePosition(position.x, position.y);
    SDL_Rect dst = { realPosition.x, realPosition.y, grid->getTileSize(), grid->getTileSize() };
    texture->draw(renderer, NULL, &dst);

    if(selected) {
        selectedTexture->draw(renderer, NULL, &dst);
    }

    for(auto weapon : weapons) {
        weapon->draw(renderer);
    }
}

void Entity::update(const Uint32& timeSinceLastFrame, bool& quit) {
    for(auto weapon : weapons) {
        weapon->update(timeSinceLastFrame);
    }

    additionalUpdate(timeSinceLastFrame, quit);
    
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
}

void Entity::setSelected(bool selected) {
    this->selected = selected;
}

bool Entity::isSelected(void) const {
    return selected;
}

Entity::Stats Entity::getStats(void) const {
    return stats;
}

const float Entity::getSpeed(void) {
    return 1000.0f / (MOVES_PER_SECOND * getStats().movesPerTurn);
}

int Entity::getCurrentHP(void) const {
    return currentHP;
}

void Entity::takeDamage(const int& amount) {
    currentHP -= amount;
}

void Entity::attack(std::shared_ptr<Entity> target, std::shared_ptr<Weapon> weapon) {
    auto targetName = target->getName();

    weapon->use(position, target);
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

bool Entity::isOnTile(const int& x, const int& y) {
    return position == glm::ivec2(x, y);
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
    return getMovesLeft() > 0 || !currentWeapon->hasFinished();
}

void Entity::useMoves(const int& numMoves) {
    movesLeft -= numMoves;
    
    if(movesLeft < 0) {
        movesLeft = 0;
    }
}

void Entity::nextTurn(void) {
    movesLeft = stats.movesPerTurn;
    path.clear();
    
    for(auto weapon : weapons) {
        weapon->reset();
    }
}

void Entity::reset(void) {
    movesLeft = 0;
    currentHP = stats.hp;
    path.clear();

    for(auto weapon : weapons) {
        weapon->reset();
    }
}

void Entity::setParticipantId(int participantId) {
    this->participantId = participantId;
}

int Entity::getParticipantId(void) const {
    return participantId;
}