#include "meleeweapon.h"

MeleeWeapon::MeleeWeapon(
    Entity* owner, 
    Grid* grid,
    EntityPool* entityPool,
    Item* item,
    EventPublisher<WeaponEventData>& publisher,
    uint32_t id,
    const std::string& name, 
    const Stats& stats
) :
    Weapon(owner, grid, entityPool, item, publisher, id, name, stats)
{ }

MeleeWeapon::MeleeWeapon(
    Entity* owner, 
    Grid* grid, 
    EntityPool* entityPool,
    Item* item,
    EventPublisher<WeaponEventData>& publisher,
    const std::string& name, 
    const Stats& stats
) :
    Weapon(owner, grid, entityPool, item, publisher, name, stats)
{ }

bool MeleeWeapon::onUse(const glm::ivec2& position, const glm::ivec2& target) {
    auto entities = entityPool->getEntities();
    auto entity = Entity::filterByTile(target.x, target.y, entities, owner->getParticipantId());
    
    if(entity != nullptr) {
        entity->takeDamage(stats.damage);
        publisher.publish<WeaponEventData>({ owner, entity, this });
    }

    return true;
}

void MeleeWeapon::update(int64_t timeSinceLastFrame) {
    // no-op
}

Weapon::Type MeleeWeapon::getType(void) const {
    return Weapon::Type::MELEE;
}