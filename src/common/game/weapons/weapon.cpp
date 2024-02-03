#include "weapon.h"

Weapon::Weapon(
    Entity* owner,
    Grid* grid,
    EntityPool* entityPool,
    Item* item,
    EventPublisher<WeaponEventData>& publisher,
    uint32_t id,
    const std::string& name, 
    const Stats& stats
) :
    id(id),
    owner(owner),
    grid(grid),
    entityPool(entityPool),
    item(item),
    publisher(publisher),
    name(name),
    stats(stats),
    usesLeft(0)
{
    game_assert(owner != nullptr);
}

Weapon::Weapon(
    Entity* owner,
    Grid* grid,
    EntityPool* entityPool,
    Item* item,
    EventPublisher<WeaponEventData>& publisher,
    const std::string& name, 
    const Stats& stats
) :
    Weapon(owner, grid, entityPool, item, publisher, getNewId(), name, stats)
{ }

void Weapon::use(const glm::ivec2& position, const glm::ivec2& target) {
    if(usesLeft <= 0) {
        return;
    }

    if(!onUse(position, target)) {
        return;
    }
    
    usesLeft--;
}

bool Weapon::hasFinished(void) {
    return usesLeft <= 0;
}

bool Weapon::isAnimationInProgress(void) {
    return false;
}

void Weapon::setFinished(void) {
    usesLeft = 0;
}

void Weapon::reset(void) {
    usesLeft = stats.uses;
}

bool Weapon::isInRange(const glm::ivec2& position) {
    return glm::distance(glm::vec2(owner->getPosition()), glm::vec2(position)) <= stats.range;
}

Weapon::Stats Weapon::getStats(void) const {
    return stats;
}

int Weapon::getUsesLeft(void) const {
    return usesLeft;
}

void Weapon::setUsesLeft(int usesLeft) {
    this->usesLeft = usesLeft;
}

uint32_t Weapon::getId(void) const {
    return id;
}

std::string Weapon::getName(void) const {
    return name;
}

Entity* Weapon::getOwner(void) {
    return owner;
}

Item* Weapon::getItem(void) {
    return item;
}