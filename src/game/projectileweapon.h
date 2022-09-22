#pragma once

#include "weapon.h"
#include "projectile.h"

class ProjectileWeapon : public Weapon {
private:
    std::shared_ptr<Texture> projectileTexture;
    std::vector<std::shared_ptr<Projectile>> projectiles;
    std::vector<std::shared_ptr<Projectile>> projectilesForDeletion;

public:
    ProjectileWeapon(std::shared_ptr<GridRenderer> grid, const std::string& name, Stats stats);

    void use(glm::ivec2 position, std::shared_ptr<Entity> target);
    void draw(std::shared_ptr<SDL_Renderer> renderer);
    void update(const Uint32& timeSinceLastFrame);

    void setProjectileTexture(std::shared_ptr<Texture> projectileTexture);
};