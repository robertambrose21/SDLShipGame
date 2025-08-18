#include "meleeweapon.h"

MeleeWeapon::MeleeWeapon(
    Actor* owner, 
    ApplicationContext* context,
    Item* item,
    EventPublisher<MeleeWeaponEventData>& publisher,
    const UUID& id,
    const std::string& name,
    const DamageSource& damageSource,
    const Stats::WeaponStats& stats
) :
    Weapon(owner, context, item, publisher, id, name, damageSource, stats)
{ }

MeleeWeapon::MeleeWeapon(
    Actor* owner, 
    ApplicationContext* context,
    Item* item,
    EventPublisher<MeleeWeaponEventData>& publisher,
    const std::string& name,
    const DamageSource& damageSource,
    const Stats::WeaponStats& stats
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
    auto actors = context->getActorPool()->getActors();
    auto actor = Actor::filterByTile(target.x, target.y, actors, owner->getParticipantId());
    
    if(actor != nullptr) {
        publisher.publish<MeleeWeaponEventData>({ owner, actor, this, damageSource.apply(actor) });
    }
}

void MeleeWeapon::update(int64_t timeSinceLastFrame) {
    // no-op
}

Stats::WeaponStats::WeaponClass MeleeWeapon::getType(void) const {
    return Stats::WeaponStats::MELEE;
}