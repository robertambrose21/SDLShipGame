#include "meleeweapon.h"

MeleeWeapon::MeleeWeapon(
    std::shared_ptr<Entity> owner, 
    std::shared_ptr<Grid> grid,
    const uint32_t& id,
    const std::string& name, 
    Stats stats
) :
    Weapon(owner, grid, id, name, stats)
{ }

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

void MeleeWeapon::draw(std::shared_ptr<GraphicsContext> graphicsContext) {
    // no-op
}

void MeleeWeapon::update(const uint32_t& timeSinceLastFrame) {
    // no-op
}

Weapon::Type MeleeWeapon::getType(void) const {
    return Weapon::Type::MELEE;
}