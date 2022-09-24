#include "projectileweapon.h"

ProjectileWeapon::ProjectileWeapon(
    std::shared_ptr<GridRenderer> gridRenderer, 
    const std::string& name, 
    Stats stats,
    std::shared_ptr<Texture> eTexture
) :
    Weapon(gridRenderer, name, stats),
    eTexture(eTexture)
{ }

void ProjectileWeapon::onUse(glm::ivec2 position, std::shared_ptr<Entity> target) {
    if(projectileTexture != nullptr) {
        projectiles.push_back(std::make_shared<Projectile>(
            gridRenderer, 
            projectileTexture, 
            position, 
            target,
            Projectile::Stats { 1, 50 },
            stats.damage,
            [&](auto grid, auto entity) {
                aoe = std::make_shared<AreaOfEffect>(gridRenderer, eTexture, entity->getPosition(), 2.0f);
            }
        ));
    }
}

void ProjectileWeapon::draw(std::shared_ptr<SDL_Renderer> renderer) {
    for(auto projectile : projectiles) {
        projectile->draw(renderer);
    }

    if(aoe != nullptr) {
        aoe->draw(renderer);
    }
}

void ProjectileWeapon::update(const Uint32& timeSinceLastFrame) {
    for(auto projectile : projectiles) {
        projectile->update(timeSinceLastFrame);

        if(projectile->hasReachedTarget()) {
            projectilesForDeletion.push_back(projectile);
        }
    }

    for(auto projectile : projectilesForDeletion) {
        projectiles.erase(std::find(projectiles.begin(), projectiles.end(), projectile));
    }
    
    projectilesForDeletion.clear();
}

void ProjectileWeapon::setProjectileTexture(std::shared_ptr<Texture> projectileTexture) {
    this->projectileTexture = projectileTexture;
}