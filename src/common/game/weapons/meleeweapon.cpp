#include "meleeweapon.h"

MeleeWeapon::MeleeWeapon(
    Entity* owner, 
    ApplicationContext* context,
    Item* item,
    EventPublisher<MeleeWeaponEventData>& publisher,
    const UUID& id,
    const std::string& name,
    const DamageSource& damageSource,
    const WeaponStats& stats
) :
    Weapon(owner, context, item, publisher, id, name, damageSource, stats)
{ }

MeleeWeapon::MeleeWeapon(
    Entity* owner, 
    ApplicationContext* context,
    Item* item,
    EventPublisher<MeleeWeaponEventData>& publisher,
    const std::string& name,
    const DamageSource& damageSource,
    const WeaponStats& stats
) :
    Weapon(owner, context, item, publisher, name, damageSource, stats)
{ }

bool MeleeWeapon::onUse(const glm::ivec2& position, const glm::ivec2& target, bool isAnimationOnly) {
    if(isAnimationOnly) {
        return true;
    }

    apply(position, target);

    return true;
}

void MeleeWeapon::apply(const glm::ivec2& position, const glm::ivec2& target) {
    auto entities = context->getEntityPool()->getEntities();
    auto entity = Entity::filterByTile(target.x, target.y, entities, owner->getParticipantId());
    
    if(entity != nullptr) {
        publisher.publish<MeleeWeaponEventData>({ owner, entity, this, damageSource.apply(entity) });
    }
}

void MeleeWeapon::update(int64_t timeSinceLastFrame) {
    // no-op
}

Weapon::Type MeleeWeapon::getType(void) const {
    return Weapon::Type::MELEE;
}