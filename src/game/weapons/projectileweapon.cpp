#include "projectileweapon.h"

ProjectileWeapon::ProjectileWeapon(
    Entity* owner,
    Grid& grid,
    EventPublisher<WeaponEventData>& publisher,
    uint32_t id,
    const std::string& name, 
    const Stats& stats,
    const Projectile::Blueprint& projectileBlueprint
) :
    Weapon(owner, grid, publisher, id, name, stats),
    projectileBlueprint(projectileBlueprint)
{ }

ProjectileWeapon::ProjectileWeapon(
    Entity* owner,
    Grid& grid,
    EventPublisher<WeaponEventData>& publisher,
    const std::string& name, 
    const Stats& stats,
    const Projectile::Blueprint& projectileBlueprint
) :
    ProjectileWeapon(owner, grid, publisher, getNewId(), name, stats, projectileBlueprint)
{ }

bool ProjectileWeapon::onUse(const glm::ivec2& position, const glm::ivec2& target) {
    if(!isInRange(target)) {
        return false;
    }

    auto& projectilePool = Application::getContext().getProjectilePool();

    projectilePool.add(
        Projectile::create(grid, projectilePool, owner->getParticipantId(), projectileBlueprint, position, target, stats.damage),
        owner
    );

    return true;
}

void ProjectileWeapon::draw(GraphicsContext& graphicsContext) {
    // no-op
}

void ProjectileWeapon::update(uint32_t timeSinceLastFrame) {
    // no-op
}

bool ProjectileWeapon::isInRange(const glm::ivec2& target) {
    return Weapon::isInRange(target) && !grid.hasIntersection(owner->getPosition(), target);
}

void ProjectileWeapon::setProjectileBlueprint(const Projectile::Blueprint& projectileBlueprint) {
    this->projectileBlueprint = projectileBlueprint;
}

Projectile::Blueprint ProjectileWeapon::getProjectileBluePrint(void) const {
    return projectileBlueprint;
}

bool ProjectileWeapon::isAnimationInProgress(void) {
    return Application::getContext().getProjectilePool().getNumProjectilesForOwner(owner) > 0;
}

Weapon::Type ProjectileWeapon::getType(void) const {
    return Weapon::Type::PROJECTILE;
}