#include "entity.h"
#include "game/application/application.h"

Entity::Entity(
    Grid* grid,
    uint32_t id,
    EventPublisher<EntityEventData>& publisher,
    const std::string& name,
    const EntityBaseStats& stats
) :
    id(id),
    publisher(publisher),
    name(name),
    stats(stats),
    currentStats(stats),
    grid(grid),
    position({ 0, 0 }),
    timeSinceLastMoved(0),
    selected(false),
    frozenForNumTurns(0)
{ }

Entity::Entity(
    Grid* grid,
    EventPublisher<EntityEventData>& publisher,
    const std::string& name,
    const EntityBaseStats& stats
) : Entity(grid, getNewId(), publisher, name, stats)
{ }

void Entity::setTextureId(uint32_t textureId) {
    this->textureId = textureId;
}

uint32_t Entity::getTextureId(void) const {
    return textureId;
}

uint32_t Entity::getSelectedTextureId(void) const {
    return selectedTextureId;
}

void Entity::setSelectedTextureId(uint32_t selectedTextureId) {
    game_assert(textureId != selectedTextureId);
    this->selectedTextureId = selectedTextureId;
}

void Entity::setColour(const Colour& colour) {
    this->colour = colour;
}

Entity::Colour Entity::getColour(void) const {
    return colour;
}

void Entity::update(int64_t timeSinceLastFrame, bool& quit) {
    for(auto& [_, weapon] : weapons) {
        weapon->update(timeSinceLastFrame);
    }

    if(frozenForNumTurns > 0) {
        return;
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

EntityBaseStats Entity::getBaseStats(void) const {
    return stats;
}

EntityCurrentStats Entity::getCurrentStats(void) const {
    return currentStats;
}

const float Entity::getSpeed(void) {
    return 2000.0f / (MOVES_PER_SECOND * getCurrentStats().movesPerTurn);
}

int Entity::getCurrentHP(void) const {
    return currentStats.totalHP;
}

void Entity::setCurrentHP(int hp) {
    currentStats.totalHP = hp;
}

void Entity::takeDamage(int amount) {
    currentStats.totalHP -= amount;
}

void Entity::attack(const glm::ivec2& target, uint32_t weaponId) {
    weapons[weaponId]->use(position, target);
}

std::vector<Weapon*> Entity::getWeapons(void) const {
    std::vector<Weapon*> vWeapons;
    
    for(auto& [_, weapon] : weapons) {
        if(weapon != nullptr) {
            vWeapons.push_back(weapon.get());
        }
    }

    return vWeapons;
}

Weapon* Entity::getWeapon(uint32_t weaponId) {
    return weapons[weaponId].get();
}

bool Entity::hasWeapon(uint32_t weaponId) {
    return weapons.contains(weaponId);
}

Weapon* Entity::addWeapon(std::unique_ptr<Weapon> weapon) {
    auto id = weapon->getId();
    weapons[id] = std::move(weapon);
    return weapons[id].get();
}

void Entity::removeWeapon(uint32_t weaponId) {
    weapons.erase(weaponId);
}

void Entity::removeAllWeapons(void) {
    weapons.clear();
}

void Entity::setCurrentWeapon(uint32_t weaponId) {
    currentWeapon = weapons[weaponId].get();
}

Weapon* Entity::getCurrentWeapon(void) {
    return currentWeapon;
}

uint32_t Entity::getId(void) const {
    return id;
}

void Entity::setId(uint32_t id) {
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

bool Entity::isOnTile(int x, int y) {
    return position == glm::ivec2(x, y);
}

void Entity::setPosition(const glm::ivec2& position) {
    this->position = position;
}

int Entity::findPath(const glm::ivec2& target, int stopShortSteps) {
    auto path = grid->findPath(getPosition(), target);

    if(path.empty()) {
        return 0;
    }

    // Remove the initial path node which is just the entities current position
    path.pop_front(); 

    if(path.size() >= stopShortSteps) {
        for(auto i = 0; i < stopShortSteps; i++) {
            path.pop_back();
        }
    }

    this->path = path;

    return path.size();
}

bool Entity::hasPath(void) {
    return !path.empty();
}

bool Entity::isNeighbour(Entity* entity) const {
    // TODO: This could be moe efficient
    return glm::distance(glm::vec2(getPosition()), glm::vec2(entity->getPosition())) < 2;
}

int Entity::getMovesLeft(void) const {
    return currentStats.movesLeft;
}

void Entity::setMovesLeft(int movesLeft) {
    currentStats.movesLeft = movesLeft;
}

int Entity::getAggroRange(void) const {
    return 20; // temp hardcoded for now
}

bool Entity::isTurnInProgress(void) const {
    return (currentWeapon != nullptr && !currentWeapon->hasFinished()) || getMovesLeft() > 0;
}

bool Entity::hasAnimationsInProgress(void) {
    return currentWeapon != nullptr && currentWeapon->isAnimationInProgress();
}

void Entity::useMoves(int numMoves) {
    currentStats.movesLeft -= numMoves;
    
    if(currentStats.movesLeft <= 0) {
        currentStats.movesLeft = 0;
        path.clear();
    }
}

void Entity::nextTurn(void) {
    currentStats.movesLeft = 0;
    path.clear();

    for(auto& [_, weapon] : weapons) {
        if(weapon != nullptr) {
            weapon->setUsesLeft(0);
        }
    }

    if(frozenForNumTurns > 0) {
        frozenForNumTurns--;
        publisher.publish({ this, "Freeze" });
    }
}

void Entity::endTurn(void) {
    currentStats.movesLeft = 0;
    path.clear();

    for(auto& [_, weapon] : weapons) {
        weapon->setUsesLeft(0);
    }
}

void Entity::setParticipantId(int participantId) {
    this->participantId = participantId;
}

int Entity::getParticipantId(void) const {
    return participantId;
}

int Entity::getFrozenFor(void) const {
    return frozenForNumTurns > 0;
}

void Entity::setFrozenFor(int numTurns) {
    frozenForNumTurns = numTurns;
}