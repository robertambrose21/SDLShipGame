#pragma once

#include "weapon.h"
#include "projectile.h"
#include "areaofeffect.h"
#include "game/application/application.h"

class ProjectileWeapon : public Weapon {
private:
    Projectile::Blueprint projectileBlueprint;
    std::shared_ptr<Entity> owner;
    
    void onUse(glm::ivec2 position, std::shared_ptr<Entity> target);

public:
    ProjectileWeapon(
        std::shared_ptr<Entity> owner,
        std::shared_ptr<GridRenderer> gridRenderer, 
        const std::string& name, 
        Stats stats, 
        Projectile::Blueprint projectileBlueprint
    );
    
    void draw(std::shared_ptr<SDL_Renderer> renderer);
    void update(const Uint32& timeSinceLastFrame);
    bool hasFinished(void);

    void setProjectileBlueprint(Projectile::Blueprint projectileBlueprint);
};