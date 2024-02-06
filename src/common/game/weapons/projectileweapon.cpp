#include "projectileweapon.h"

ProjectileWeapon::ProjectileWeapon(
    Entity* owner,
    Grid* grid,
    EntityPool* entityPool,
    ProjectilePool* projectilePool,
    Item* item,
    EventPublisher<WeaponEventData>& publisher,
    const UUID& id,
    const std::string& name, 
    const Stats& stats,
    const DamageSource& damageSource,
    const Projectile::Blueprint& projectileBlueprint
) :
    Weapon(owner, grid, entityPool, item, publisher, id, name, stats, damageSource),
    projectilePool(projectilePool),
    projectileBlueprint(projectileBlueprint)
{ }

ProjectileWeapon::ProjectileWeapon(
    Entity* owner,
    Grid* grid,
    EntityPool* entityPool,
    ProjectilePool* projectilePool,
    Item* item,
    EventPublisher<WeaponEventData>& publisher,
    const std::string& name, 
    const Stats& stats,
    const DamageSource& damageSource,
    const Projectile::Blueprint& projectileBlueprint
) :
    ProjectileWeapon(owner, grid, entityPool, projectilePool, item, publisher, UUID::getNewUUID(), name, stats, damageSource, projectileBlueprint)
{ }

bool ProjectileWeapon::onUse(const glm::ivec2& position, const glm::ivec2& target) {
    if(!isInRange(target)) {
        return false;
    }

    projectilePool->add(
        Projectile::create(
            grid,
            entityPool,
            *projectilePool, 
            owner->getParticipantId(), 
            projectileBlueprint, 
            position, 
            target, 
            damageSource
        ), 
        owner
    );

    return true;
}

void ProjectileWeapon::update(int64_t timeSinceLastFrame) {
    // no-op
}

bool ProjectileWeapon::isInRange(const glm::ivec2& target) {
    return Weapon::isInRange(target) && !grid->hasIntersection(owner->getPosition(), target);
}

void ProjectileWeapon::setProjectileBlueprint(const Projectile::Blueprint& projectileBlueprint) {
    this->projectileBlueprint = projectileBlueprint;
}

Projectile::Blueprint ProjectileWeapon::getProjectileBluePrint(void) const {
    return projectileBlueprint;
}

bool ProjectileWeapon::isAnimationInProgress(void) {
    return projectilePool->getNumProjectilesForOwner(owner) > 0;
}

Weapon::Type ProjectileWeapon::getType(void) const {
    return Weapon::Type::PROJECTILE;
}