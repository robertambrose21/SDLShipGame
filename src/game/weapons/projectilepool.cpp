#include "projectilepool.h"

ProjectilePool::ProjectilePool()
{ }

void ProjectilePool::add(std::shared_ptr<Projectile> projectile, std::shared_ptr<Entity> owner) {
    projectiles[owner].push_back(projectile);
}

void ProjectilePool::draw(std::shared_ptr<GraphicsContext> graphicsContext) {
    for(auto [owner, projectilesForOwner] : projectiles) {
        for(auto projectile : projectilesForOwner) {
            projectile->draw(graphicsContext);
        }
    }
}

void ProjectilePool::update(const Uint32& timeSinceLastFrame) {
    for(auto [owner, projectilesForOwner] : projectiles) {
        for(auto projectile : projectilesForOwner) {
            projectile->update(timeSinceLastFrame);

            if(projectile->hasReachedTarget()) {
                projectilesForDeletion[owner].push_back(projectile);
            }
        }
    }

    for(auto [owner, projectilesForOwner] : projectilesForDeletion) {
        for(auto projectile : projectilesForOwner) {
            projectiles[owner].erase(std::find(projectiles[owner].begin(), projectiles[owner].end(), projectile));
        }
    }
    
    projectilesForDeletion.clear();
}

std::vector<std::shared_ptr<Projectile>> ProjectilePool::getProjectilesForOwner(std::shared_ptr<Entity> owner) {
    return projectiles[owner];
}