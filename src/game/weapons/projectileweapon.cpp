#include "projectileweapon.h"

ProjectileWeapon::ProjectileWeapon(
    Entity* owner,
    Grid& grid, 
    uint32_t id,
    const std::string& name, 
    const Stats& stats,
    const Projectile::Blueprint& projectileBlueprint
) :
    Weapon(owner, grid, id, name, stats),
    projectileBlueprint(projectileBlueprint)
{ }

ProjectileWeapon::ProjectileWeapon(
    Entity* owner,
    Grid& grid, 
    const std::string& name, 
    const Stats& stats,
    const Projectile::Blueprint& projectileBlueprint
) :
    ProjectileWeapon(owner, grid, getNewId(), name, stats, projectileBlueprint)
{ }

bool ProjectileWeapon::onUse(const glm::ivec2& position, const glm::ivec2& target) {
    if(glm::distance(glm::vec2(position), glm::vec2(target)) > stats.range) {
        return false;
    }

    Application::getContext().getProjectilePool().add(
        Projectile::create(grid, owner->getParticipantId(), projectileBlueprint, position, target, stats.damage),
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

void ProjectileWeapon::setProjectileBlueprint(const Projectile::Blueprint& projectileBlueprint) {
    this->projectileBlueprint = projectileBlueprint;
}

Projectile::Blueprint ProjectileWeapon::getProjectileBluePrint(void) const {
    return projectileBlueprint;
}

bool ProjectileWeapon::hasFinished(void) {
    auto activeProjectiles = Application::getContext().getProjectilePool().getNumProjectilesForOwner(owner);

    return Weapon::hasFinished() && activeProjectiles == 0;
}

Weapon::Type ProjectileWeapon::getType(void) const {
    return Weapon::Type::PROJECTILE;
}