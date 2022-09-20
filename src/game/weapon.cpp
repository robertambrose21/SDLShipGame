#include "weapon.h"

Weapon::Weapon(const std::string& name, Stats stats) :
    name(name),
    stats(stats)
{
    onHitEvent = [&]() {
        std::cout << "On hit event triggered" << std::endl;
    };
}

void Weapon::use(std::shared_ptr<Entity> target) {
    target->takeDamage(stats.damage);
    onHitEvent();
}

Weapon::Stats Weapon::getStats(void) const {
    return stats;
}

std::string Weapon::getName(void) const {
    return name;
}