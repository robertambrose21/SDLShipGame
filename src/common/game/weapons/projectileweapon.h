#pragma once

#include "weapon.h"
#include "projectile.h"
#include "areaofeffect.h"
#include "game/application/application.h"

class ProjectileWeapon : public Weapon {
private:
    Projectile::Blueprint projectileBlueprint;

    bool onUse(const glm::ivec2& position, const glm::ivec2& target);

public:
    ProjectileWeapon(
        Entity* owner,
        Grid& grid,
        EventPublisher<WeaponEventData>& publisher,
        uint32_t id,
        const std::string& name, 
        const Stats& stats, 
        const Projectile::Blueprint& projectileBlueprint
    );

    ProjectileWeapon(
        Entity* owner,
        Grid& grid,
        EventPublisher<WeaponEventData>& publisher,
        const std::string& name, 
        const Stats& stats, 
        const Projectile::Blueprint& projectileBlueprint
    );
    
    void update(int64_t timeSinceLastFrame);
    bool isAnimationInProgress(void);
    Type getType(void) const;

    bool isInRange(const glm::ivec2& target);

    void setProjectileBlueprint(const Projectile::Blueprint& projectileBlueprint);
    Projectile::Blueprint getProjectileBluePrint(void) const;
};