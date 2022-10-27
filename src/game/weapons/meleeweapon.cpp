#include "meleeweapon.h"

MeleeWeapon::MeleeWeapon(
    std::shared_ptr<Entity> owner, 
    std::shared_ptr<Grid> grid, 
    const std::string& name, 
    Stats stats
) :
    Weapon(owner, grid, name, stats)
{ }

void MeleeWeapon::onUse(glm::ivec2 position, std::shared_ptr<Entity> target) {
    target->takeDamage(stats.damage);
}

void MeleeWeapon::draw(std::shared_ptr<SDL_Renderer> renderer, std::shared_ptr<GridRenderer> gridRenderer) {
    // no-op
}

void MeleeWeapon::update(const Uint32& timeSinceLastFrame) {
    // no-op
}