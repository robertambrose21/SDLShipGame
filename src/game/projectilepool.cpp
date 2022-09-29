#include "projectilepool.h"

ProjectilePool::ProjectilePool()
{ }

void ProjectilePool::add(std::shared_ptr<Projectile> projectile) {
    projectiles.push_back(projectile);
}

void ProjectilePool::draw(std::shared_ptr<SDL_Renderer> renderer) {
    for(auto projectile : projectiles) {
        projectile->draw(renderer);
    }
}

void ProjectilePool::update(const Uint32& timeSinceLastFrame) {
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