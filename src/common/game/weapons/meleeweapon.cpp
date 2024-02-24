#include "meleeweapon.h"

MeleeWeapon::MeleeWeapon(
    Entity* owner, 
    Grid* grid,
    EntityPool* entityPool,
    Item* item,
    EventPublisher<WeaponEventData>& publisher,
    const UUID& id,
    const std::string& name, 
    const AllStats& stats,
    const DamageSource& damageSource
) :
    Weapon(owner, grid, entityPool, item, publisher, id, name, stats, damageSource)
{ }

MeleeWeapon::MeleeWeapon(
    Entity* owner, 
    Grid* grid, 
    EntityPool* entityPool,
    Item* item,
    EventPublisher<WeaponEventData>& publisher,
    const std::string& name, 
    const AllStats& stats,
    const DamageSource& damageSource
) :
    Weapon(owner, grid, entityPool, item, publisher, name, stats, damageSource)
{ }

bool MeleeWeapon::onUse(const glm::ivec2& position, const glm::ivec2& target) {
    auto entities = entityPool->getEntities();
    auto entity = Entity::filterByTile(target.x, target.y, entities, owner->getParticipantId());
    
    if(entity != nullptr) {
        publisher.publish<WeaponEventData>({ owner, entity, this, damageSource.apply(entity) });
    }

    return true;
}

void MeleeWeapon::update(int64_t timeSinceLastFrame) {
    // no-op
}

Weapon::Type MeleeWeapon::getType(void) const {
    return Weapon::Type::MELEE;
}