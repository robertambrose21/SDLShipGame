#pragma once

#include "projectile.h"

class ProjectilePool {
private:
    std::vector<std::shared_ptr<Projectile>> projectiles;
    std::vector<std::shared_ptr<Projectile>> projectilesForDeletion;

public:
    ProjectilePool();

    void add(std::shared_ptr<Projectile> projectile);

    void draw(std::shared_ptr<SDL_Renderer> renderer);
    void update(const Uint32& timeSinceLastFrame);
};