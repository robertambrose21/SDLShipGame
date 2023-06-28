#include "meleeweapon.h"

MeleeWeapon::MeleeWeapon(
    const std::shared_ptr<Entity>& owner, 
    const std::shared_ptr<Grid>& grid,
    uint32_t id,
    const std::string& name, 
    const Stats& stats
) :
    Weapon(owner, grid, id, name, stats)
{ }

MeleeWeapon::MeleeWeapon(
    const std::shared_ptr<Entity>& owner, 
    const std::shared_ptr<Grid>& grid, 
    const std::string& name, 
    const Stats& stats
) :
    Weapon(owner, grid, name, stats)
{ }

bool MeleeWeapon::onUse(const glm::ivec2& position, const std::shared_ptr<Entity>& target) {
    game_assert(target != nullptr);
    target->takeDamage(stats.damage);
    return true;
}

bool MeleeWeapon::onUse(const glm::ivec2& position, const glm::ivec2& target) {
    auto entities = Application::getContext()->getEntityPool()->getEntities();
    auto entity = Entity::filterByTile(position.x, position.y, entities, owner->getId());
    
    if(entity != nullptr) {
        entity->takeDamage(stats.damage);
    }

    return true;
}

void MeleeWeapon::draw(const std::shared_ptr<GraphicsContext>& graphicsContext) {
    // no-op
}

void MeleeWeapon::update(uint32_t timeSinceLastFrame) {
    // no-op
}

Weapon::Type MeleeWeapon::getType(void) const {
    return Weapon::Type::MELEE;
}