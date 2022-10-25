#include "weapon.h"

Weapon::Weapon(
    std::shared_ptr<Entity> owner,
    std::shared_ptr<GridRenderer> gridRenderer, 
    const std::string& name, 
    Stats stats
) :
    owner(owner),
    gridRenderer(gridRenderer),
    name(name),
    stats(stats),
    usesLeft(stats.uses)
{ }

void Weapon::use(glm::ivec2 position, std::shared_ptr<Entity> target) {
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

bool Weapon::isInRange(glm::ivec2 position) {
    return glm::distance(glm::vec2(owner->getPosition()), glm::vec2(position)) <= stats.range;
}

Weapon::Stats Weapon::getStats(void) const {
    return stats;
}

int Weapon::getUsesLeft(void) const {
    return usesLeft;
}

std::string Weapon::getName(void) const {
    return name;
}

std::shared_ptr<Entity> Weapon::getOwner(void) {
    return owner;
}