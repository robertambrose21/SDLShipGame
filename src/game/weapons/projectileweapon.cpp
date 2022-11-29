#include "projectileweapon.h"

ProjectileWeapon::ProjectileWeapon(
    std::shared_ptr<Entity> owner,
    std::shared_ptr<Grid> grid, 
    const uint32_t& id,
    const std::string& name, 
    Stats stats,
    Projectile::Blueprint projectileBlueprint
) :
    Weapon(owner, grid, id, name, stats),
    projectileBlueprint(projectileBlueprint)
{ }

ProjectileWeapon::ProjectileWeapon(
    std::shared_ptr<Entity> owner,
    std::shared_ptr<Grid> grid, 
    const std::string& name, 
    Stats stats,
    Projectile::Blueprint projectileBlueprint
) :
    ProjectileWeapon(owner, grid, getNewId(), name, stats, projectileBlueprint)
{ }

void ProjectileWeapon::onUse(glm::ivec2 position, std::shared_ptr<Entity> target) {
    Application::getContext()->getProjectilePool()->add(
        Projectile::create(grid, projectileBlueprint, position, target, stats.damage),
        owner
    );
}

void ProjectileWeapon::draw(std::shared_ptr<GraphicsContext> graphicsContext) {
    // no-op
}

void ProjectileWeapon::update(const uint32_t& timeSinceLastFrame) {
    // no-op
}

void ProjectileWeapon::setProjectileBlueprint(Projectile::Blueprint projectileBlueprint) {
    this->projectileBlueprint = projectileBlueprint;
}

Projectile::Blueprint ProjectileWeapon::getProjectileBluePrint(void) const {
    return projectileBlueprint;
}

bool ProjectileWeapon::hasFinished(void) {
    auto activeProjectiles = Application::getContext()->getProjectilePool()->getProjectilesForOwner(owner);

    return Weapon::hasFinished() && activeProjectiles.size() == 0;
}

Weapon::Type ProjectileWeapon::getType(void) const {
    return Weapon::Type::PROJECTILE;
}