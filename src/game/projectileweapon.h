#pragma once

#include "weapon.h"
#include "projectile.h"
#include "areaofeffect.h"
#include "application.h"

class ProjectileWeapon : public Weapon {
private:
    std::shared_ptr<Texture> eTexture;
    std::shared_ptr<AreaOfEffect> aoe;

    Projectile::Blueprint projectileBlueprint;
    
    void onUse(glm::ivec2 position, std::shared_ptr<Entity> target);

public:
    ProjectileWeapon(
        std::shared_ptr<GridRenderer> gridRenderer, 
        const std::string& name, 
        Stats stats, 
        Projectile::Blueprint projectileBlueprint
    );
    
    void draw(std::shared_ptr<SDL_Renderer> renderer);
    void update(const Uint32& timeSinceLastFrame);

    void setProjectileBlueprint(Projectile::Blueprint projectileBlueprint);
};