#include "projectileweapon.h"

ProjectileWeapon::ProjectileWeapon(
    std::shared_ptr<GridRenderer> gridRenderer, 
    const std::string& name, 
    Stats stats,
    Projectile::Blueprint projectileBlueprint
) :
    Weapon(gridRenderer, name, stats),
    projectileBlueprint(projectileBlueprint)
{ }

void ProjectileWeapon::onUse(glm::ivec2 position, std::shared_ptr<Entity> target) {
    Application::getContext()->getProjectilePool()->add(
        Projectile::create(projectileBlueprint, gridRenderer, position, target, stats.damage));
}

void ProjectileWeapon::draw(std::shared_ptr<SDL_Renderer> renderer) {
    if(aoe != nullptr) {
        aoe->draw(renderer);
    }
}

void ProjectileWeapon::update(const Uint32& timeSinceLastFrame) {
    // no-op
}

void ProjectileWeapon::setProjectileBlueprint(Projectile::Blueprint projectileBlueprint) {
    this->projectileBlueprint = projectileBlueprint;
}