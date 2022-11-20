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
        std::shared_ptr<Grid> grid, 
        const std::string& name, 
        Stats stats, 
        Projectile::Blueprint projectileBlueprint
    );
    
    void draw(std::shared_ptr<GraphicsContext> graphicsContext);
    void update(const uint32_t& timeSinceLastFrame);
    bool hasFinished(void);

    void setProjectileBlueprint(Projectile::Blueprint projectileBlueprint);
};