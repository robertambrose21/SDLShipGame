#include "projectileweapon.h"

ProjectileWeapon::ProjectileWeapon(
    Entity* owner,
    ApplicationContext* context,
    Item* item,
    EventPublisher<MeleeWeaponEventData>& publisher,
    const UUID& id,
    const std::string& name, 
    const AllStats& stats,
    const DamageSource& damageSource,
    const Projectile::Blueprint& projectileBlueprint
) :
    Weapon(owner, context, item, publisher, id, name, stats, damageSource),
    projectileBlueprint(projectileBlueprint)
{ }

ProjectileWeapon::ProjectileWeapon(
    Entity* owner,
    ApplicationContext* context,
    Item* item,
    EventPublisher<MeleeWeaponEventData>& publisher,
    const std::string& name, 
    const AllStats& stats,
    const DamageSource& damageSource,
    const Projectile::Blueprint& projectileBlueprint
) :
    ProjectileWeapon(owner, context, item, publisher, UUID::getNewUUID(), name, stats, damageSource, projectileBlueprint)
{ }

bool ProjectileWeapon::onUse(const glm::ivec2& position, const glm::ivec2& target, bool isAnimationOnly) {
    if(!isInRange(target)) {
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
    return Weapon::isInRange(target) && !context->getGrid()->hasIntersection(owner->getPosition(), target);
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

Weapon::Type ProjectileWeapon::getType(void) const {
    return Weapon::Type::PROJECTILE;
}