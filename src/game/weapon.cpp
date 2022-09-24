#include "weapon.h"

Weapon::Weapon(std::shared_ptr<GridRenderer> gridRenderer, const std::string& name, Stats stats) :
    gridRenderer(gridRenderer),
    name(name),
    stats(stats),
    usesLeft(stats.uses)
{ }

void Weapon::use(glm::ivec2 position, std::shared_ptr<Entity> target) {
    onUse(position, target);

    usesLeft--;
}

void Weapon::reset(void) {
    usesLeft = stats.uses;
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