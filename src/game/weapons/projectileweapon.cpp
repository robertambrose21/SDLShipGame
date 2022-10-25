#include "projectileweapon.h"

ProjectileWeapon::ProjectileWeapon(
    std::shared_ptr<Entity> owner,
    std::shared_ptr<GridRenderer> gridRenderer, 
    const std::string& name, 
    Stats stats,
    Projectile::Blueprint projectileBlueprint
) :
    Weapon(owner, gridRenderer, name, stats),
    projectileBlueprint(projectileBlueprint)
{ }

void ProjectileWeapon::onUse(glm::ivec2 position, std::shared_ptr<Entity> target) {
    Application::getContext()->getProjectilePool()->add(
        Projectile::create(projectileBlueprint, gridRenderer, position, target, stats.damage),
        owner
    );
}

void ProjectileWeapon::draw(std::shared_ptr<SDL_Renderer> renderer) {
    // no-op
}

void ProjectileWeapon::update(const Uint32& timeSinceLastFrame) {
    // no-op
}

void ProjectileWeapon::setProjectileBlueprint(Projectile::Blueprint projectileBlueprint) {
    this->projectileBlueprint = projectileBlueprint;
}

bool ProjectileWeapon::hasFinished(void) {
    auto activeProjectiles = Application::getContext()->getProjectilePool()->getProjectilesForOwner(owner);

    return Weapon::hasFinished() && activeProjectiles.size() == 0;
}