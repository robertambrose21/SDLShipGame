#include "meleeweapon.h"

MeleeWeapon::MeleeWeapon(std::shared_ptr<GridRenderer> gridRenderer, const std::string& name, Stats stats) :
    Weapon(gridRenderer, name, stats)
{ }

void MeleeWeapon::onUse(glm::ivec2 position, std::shared_ptr<Entity> target) {
    target->takeDamage(stats.damage);
}

void MeleeWeapon::draw(std::shared_ptr<SDL_Renderer> renderer) {
    // no-op
}

void MeleeWeapon::update(const Uint32& timeSinceLastFrame) {
    // no-op
}