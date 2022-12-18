#include "projectileweapon.h"

ProjectileWeapon::ProjectileWeapon(
    const std::shared_ptr<Entity>& owner,
    const std::shared_ptr<Grid>& grid, 
    uint32_t id,
    const std::string& name, 
    const Stats& stats,
    const Projectile::Blueprint& projectileBlueprint
) :
    Weapon(owner, grid, id, name, stats),
    projectileBlueprint(projectileBlueprint)
{ }

ProjectileWeapon::ProjectileWeapon(
    const std::shared_ptr<Entity>& owner,
    const std::shared_ptr<Grid>& grid, 
    const std::string& name, 
    const Stats& stats,
    const Projectile::Blueprint& projectileBlueprint
) :
    ProjectileWeapon(owner, grid, getNewId(), name, stats, projectileBlueprint)
{ }

void ProjectileWeapon::onUse(const glm::ivec2& position, const std::shared_ptr<Entity>& target) {
    Application::getContext()->getProjectilePool()->add(
        Projectile::create(grid, projectileBlueprint, position, target, stats.damage),
        owner
    );
}

void ProjectileWeapon::draw(const std::shared_ptr<GraphicsContext>& graphicsContext) {
    // no-op
}

void ProjectileWeapon::update(uint32_t timeSinceLastFrame) {
    // no-op
}

void ProjectileWeapon::setProjectileBlueprint(const Projectile::Blueprint& projectileBlueprint) {
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