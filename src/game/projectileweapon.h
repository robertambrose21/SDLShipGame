#pragma once

#include "weapon.h"
#include "projectile.h"
#include "areaofeffect.h"

class ProjectileWeapon : public Weapon {
private:
    std::shared_ptr<Texture> eTexture;
    std::shared_ptr<AreaOfEffect> aoe;

    std::shared_ptr<Texture> projectileTexture;
    std::vector<std::shared_ptr<Projectile>> projectiles;
    std::vector<std::shared_ptr<Projectile>> projectilesForDeletion;

    void onUse(glm::ivec2 position, std::shared_ptr<Entity> target);

public:
    ProjectileWeapon(std::shared_ptr<GridRenderer> gridRenderer, const std::string& name, Stats stats, std::shared_ptr<Texture> eTexture);
    
    void draw(std::shared_ptr<SDL_Renderer> renderer);
    void update(const Uint32& timeSinceLastFrame);

    void setProjectileTexture(std::shared_ptr<Texture> projectileTexture);
};