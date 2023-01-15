#pragma once

#include "weapon.h"
#include "projectile.h"
#include "areaofeffect.h"
#include "game/application/application.h"

class ProjectileWeapon : public Weapon {
private:
    Projectile::Blueprint projectileBlueprint;
    
    bool onUse(const glm::ivec2& position, const std::shared_ptr<Entity>& target);

public:
    ProjectileWeapon(
        const std::shared_ptr<Entity>& owner,
        const std::shared_ptr<Grid>& grid,
        uint32_t id,
        const std::string& name, 
        const Stats& stats, 
        const Projectile::Blueprint& projectileBlueprint
    );

    ProjectileWeapon(
        const std::shared_ptr<Entity>& owner,
        const std::shared_ptr<Grid>& grid, 
        const std::string& name, 
        const Stats& stats, 
        const Projectile::Blueprint& projectileBlueprint
    );
    
    void draw(const std::shared_ptr<GraphicsContext>& graphicsContext);
    void update(uint32_t timeSinceLastFrame);
    bool hasFinished(void);
    Type getType(void) const;

    void setProjectileBlueprint(const Projectile::Blueprint& projectileBlueprint);
    Projectile::Blueprint getProjectileBluePrint(void) const;
};