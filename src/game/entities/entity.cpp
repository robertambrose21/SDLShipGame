#include "entity.h"
#include "game/application/application.h"

Entity::Entity(
    const uint32_t& id,
    const std::string& name,
    Stats stats
) :
    id(id),
    name(name),
    stats(stats),
    position({ 0, 0 }),
    movesLeft(0),
    currentHP(stats.hp),
    timeSinceLastMoved(0),
    selected(false)
{
    grid = Application::getContext()->getGrid();
}

Entity::Entity(
    const std::string& name,
    Stats stats
) : Entity(getNewId(), name, stats)
{ }

void Entity::setTextureId(const uint8_t& textureId) {
    this->textureId = textureId;
}

uint8_t Entity::getTextureId(void) const {
    return textureId;
}

void Entity::setSelectedTextureId(const uint8_t& selectedTextureId) {
    game_assert(textureId != selectedTextureId);
    this->selectedTextureId = selectedTextureId;
}

void Entity::draw(std::shared_ptr<GraphicsContext> graphicsContext) {
    game_assert(graphicsContext != nullptr);

    graphicsContext->getGridRenderer()->draw(graphicsContext, textureId, position);

    if(selected) {
        graphicsContext->getGridRenderer()->draw(graphicsContext, selectedTextureId, position);
    }

    for(auto [_, weapon] : weapons) {
        weapon->draw(graphicsContext);
    }
}

void Entity::update(const uint32_t& timeSinceLastFrame, bool& quit) {
    for(auto [_, weapon] : weapons) {
        weapon->update(timeSinceLastFrame);
    }

    if(behaviourStrategy != nullptr) {
        behaviourStrategy->onUpdate(timeSinceLastFrame, quit);
    }
    
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

std::shared_ptr<BehaviourStrategy> Entity::getBehaviourStrategy(void) {
    return behaviourStrategy;
}

void Entity::setBehaviourStrategy(std::shared_ptr<BehaviourStrategy> behaviourStrategy) {
    this->behaviourStrategy = behaviourStrategy;
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

void Entity::setCurrentHP(const int& hp) {
    this->currentHP = hp;
}

void Entity::takeDamage(const int& amount) {
    currentHP -= amount;
}

void Entity::attack(std::shared_ptr<Entity> target, std::shared_ptr<Weapon> weapon) {
    auto targetName = target->getName();

    weapon->use(position, target);
}

std::map<uint32_t, std::shared_ptr<Weapon>> Entity::getWeapons(void) const {
    return weapons;
}

std::shared_ptr<Weapon> Entity::getWeapon(const uint32_t& weaponId) {
    game_assert(weapons.contains(weaponId));
    return weapons[weaponId];
}

bool Entity::hasWeapon(const uint32_t& weaponId) {
    return weapons.contains(weaponId);
}

std::shared_ptr<Weapon> Entity::addWeapon(std::shared_ptr<Weapon> weapon) {
    weapons[weapon->getId()] = weapon;
    return weapon;
}

void Entity::removeWeapon(const std::string& name) {
    for(auto [weaponId, weapon] : weapons) {
        if(weapon->getName() == name) {
            weapons.erase(weaponId);
        }
    }
}

void Entity::setCurrentWeapon(std::shared_ptr<Weapon> weapon) {
    currentWeapon = weapon;
}

std::shared_ptr<Weapon> Entity::getCurrentWeapon(void) {
    return currentWeapon;
}

uint32_t Entity::getId(void) const {
    return id;
}

void Entity::setId(const uint32_t& id) {
    this->id = id;
}

std::string Entity::getName(void) const {
    return name;
}

void Entity::setName(const std::string& name) {
    this->name = name;
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
    auto path = grid->findPath(getPosition(), target);

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

bool Entity::hasPath(void) {
    return !path.empty();
}

bool Entity::isNeighbour(std::shared_ptr<Entity> entity) const {
    return glm::distance(glm::vec2(getPosition()), glm::vec2(entity->getPosition())) < 2;
}

int Entity::getMovesLeft(void) const {
    return movesLeft;
}

void Entity::setMovesLeft(const int& movesLeft) {
    this->movesLeft = movesLeft;
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
    
    for(auto [_, weapon] : weapons) {
        weapon->reset();
    }

    if(behaviourStrategy != nullptr) {
        behaviourStrategy->onNextTurn();
    }
}

bool Entity::endTurnCondition(void) {
    return behaviourStrategy == nullptr ? false : behaviourStrategy->endTurnCondition();
}

void Entity::reset(void) {
    movesLeft = 0;
    currentHP = stats.hp;
    path.clear();

    for(auto [_, weapon] : weapons) {
        weapon->reset();
    }
}

void Entity::setParticipantId(int participantId) {
    this->participantId = participantId;
}

int Entity::getParticipantId(void) const {
    return participantId;
}