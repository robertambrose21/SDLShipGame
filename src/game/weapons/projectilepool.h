#pragma once

#include <map>
#include <vector>

#include "projectile.h"
#include "game/entities/entity.h"

class ProjectilePool {
private:
    std::map<std::shared_ptr<Entity>, std::vector<std::shared_ptr<Projectile>>> projectiles;
    std::map<std::shared_ptr<Entity>, std::vector<std::shared_ptr<Projectile>>> projectilesForDeletion;

public:
    ProjectilePool();

    void add(std::shared_ptr<Projectile> projectile, std::shared_ptr<Entity> owner);

    void draw(std::shared_ptr<GraphicsContext> graphicsContext);
    void update(const uint32_t& timeSinceLastFrame);

    std::vector<std::shared_ptr<Projectile>> getProjectilesForOwner(std::shared_ptr<Entity> owner);
};