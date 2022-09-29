#pragma once

#include <map>
#include <vector>

#include "projectile.h"
#include "entity.h"

class ProjectilePool {
private:
    std::map<std::shared_ptr<Entity>, std::vector<std::shared_ptr<Projectile>>> projectiles;
    std::map<std::shared_ptr<Entity>, std::vector<std::shared_ptr<Projectile>>> projectilesForDeletion;

public:
    ProjectilePool();

    void add(std::shared_ptr<Projectile> projectile, std::shared_ptr<Entity> owner);

    void draw(std::shared_ptr<SDL_Renderer> renderer);
    void update(const Uint32& timeSinceLastFrame);

    std::vector<std::shared_ptr<Projectile>> getProjectilesForOwner(std::shared_ptr<Entity> owner);
};