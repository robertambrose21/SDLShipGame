#include "weapon.h"

Weapon::Weapon(
    const std::shared_ptr<Entity>& owner,
    const std::shared_ptr<Grid>& grid,
    const uint32_t& id,
    const std::string& name, 
    const Stats& stats
) :
    id(id),
    owner(owner),
    grid(grid),
    name(name),
    stats(stats),
    usesLeft(stats.uses)
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

void Weapon::use(const glm::ivec2& position, const std::shared_ptr<Entity>& target) {
    if(usesLeft > 0) {
        onUse(position, target);
        usesLeft--;

        std::cout
        << "["
        << owner->getName()
        << "] attacked the [" 
        << target->getName()
        << "] for [" 
        << getStats().damage 
        << "] damage with ["
        << getName()
        << "], ["
        << target->getName()
        << "] now has [" 
        << (target == nullptr ? 0 : target->getCurrentHP() - getStats().damage)
        << "] hp" 
        << std::endl;
    }
}

bool Weapon::hasFinished(void) {
    return usesLeft <= 0;
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

void Weapon::setUsesLeft(const int& usesLeft) {
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