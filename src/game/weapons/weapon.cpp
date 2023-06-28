#include "weapon.h"

Weapon::Weapon(
    const std::shared_ptr<Entity>& owner,
    const std::shared_ptr<Grid>& grid,
    uint32_t id,
    const std::string& name, 
    const Stats& stats
) :
    id(id),
    owner(owner),
    grid(grid),
    name(name),
    stats(stats),
    usesLeft(0)
{
    game_assert(owner != nullptr);
    game_assert(grid != nullptr);
}

Weapon::Weapon(
    const std::shared_ptr<Entity>& owner,
    const std::shared_ptr<Grid>& grid, 
    const std::string& name, 
    const Stats& stats
) :
    Weapon(owner, grid, getNewId(), name, stats)
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
    return true;
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

std::shared_ptr<Entity> Weapon::getOwner(void) {
    return owner;
}