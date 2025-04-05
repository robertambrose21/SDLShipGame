#include "projectileweapon.h"

ProjectileWeapon::ProjectileWeapon(
    Entity* owner,
    ApplicationContext* context,
    Item* item,
    EventPublisher<MeleeWeaponEventData>& publisher,
    const UUID& id,
    const std::string& name, 
    const Stats::WeaponStats& stats,
    const DamageSource& damageSource,
    const Projectile::Blueprint& projectileBlueprint
) :
    Weapon(owner, context, item, publisher, id, name, damageSource, stats),
    projectileBlueprint(projectileBlueprint)
{ }

ProjectileWeapon::ProjectileWeapon(
    Entity* owner,
    ApplicationContext* context,
    Item* item,
    EventPublisher<MeleeWeaponEventData>& publisher,
    const std::string& name, 
    const Stats::WeaponStats& stats,
    const DamageSource& damageSource,
    const Projectile::Blueprint& projectileBlueprint
) :
    ProjectileWeapon(owner, context, item, publisher, UUID::getNewUUID(), name, stats, damageSource, projectileBlueprint)
{ }

bool ProjectileWeapon::onUse(const glm::ivec2& position, const glm::ivec2& target, bool isAnimationOnly) {
    if(!isInRange(target)) {
        spdlog::trace("Cannot use weapon {}#{}, out of range", name, id.getString());
        return false;
    }

    context->getProjectilePool()->add(
        Projectile::create(
            context,
            *context->getProjectilePool(), 
            owner->getParticipantId(), 
            projectileBlueprint, 
            position, 
            target,
            damageSource,
            isAnimationOnly
        ), 
        owner
    );

    return true;
}

void ProjectileWeapon::apply(const glm::ivec2& position, const glm::ivec2& target) {
    // no-op
}

void ProjectileWeapon::update(int64_t timeSinceLastFrame) {
    // no-op
}

bool ProjectileWeapon::isInRange(const glm::ivec2& target) {
    if(!Weapon::isInRange(target)) {
        return false;
    }

    glm::vec2 ownerCentrePos = glm::vec2(owner->getPosition()) + glm::vec2(.5f, .5f);
    glm::vec2 targetCentrePos = glm::vec2(target) + glm::vec2(.5f, .5f);

    return !context->getGrid()->hasIntersection(ownerCentrePos, targetCentrePos);
}

void ProjectileWeapon::setProjectileBlueprint(const Projectile::Blueprint& projectileBlueprint) {
    this->projectileBlueprint = projectileBlueprint;
}

Projectile::Blueprint ProjectileWeapon::getProjectileBluePrint(void) const {
    return projectileBlueprint;
}

bool ProjectileWeapon::isAnimationInProgress(void) {
    return context->getProjectilePool()->getNumProjectilesForOwner(owner) > 0;
}

Stats::WeaponStats::WeaponClass ProjectileWeapon::getType(void) const {
    return Stats::WeaponStats::PROJECTILE;
}