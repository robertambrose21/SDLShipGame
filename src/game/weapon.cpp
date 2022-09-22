#include "weapon.h"

Weapon::Weapon(std::shared_ptr<GridRenderer> grid, const std::string& name, Stats stats) :
    grid(grid),
    name(name),
    stats(stats)
{ }

Weapon::Stats Weapon::getStats(void) const {
    return stats;
}

std::string Weapon::getName(void) const {
    return name;
}