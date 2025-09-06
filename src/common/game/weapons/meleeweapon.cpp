#include "meleeweapon.h"

MeleeWeapon::MeleeWeapon(
    entt::entity owner, 
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
    entt::entity owner, 
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
    auto& ownerActor = context->getEntityRegistry().get<Actor>(owner);

    auto entities = context->getActorPool()->filterByTile(target.x, target.y, ownerActor.getParticipantId());
    
    for(auto entity : entities) {
        auto& actor = context->getEntityRegistry().get<Actor>(entity);
        auto const& stats = context->getEntityRegistry().get<Stats::ActorStats>(entity);
        auto damage = damageSource.rollActorDamage(stats);
        context->getActorController()->applyDamage(entity, damage);

        publisher.publish<MeleeWeaponEventData>({ &ownerActor, &actor, this, damage });
    }
}

void MeleeWeapon::update(int64_t timeSinceLastFrame) {
    // no-op
}

Stats::WeaponStats::WeaponClass MeleeWeapon::getType(void) const {
    return Stats::WeaponStats::MELEE;
}