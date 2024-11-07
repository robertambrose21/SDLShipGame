#pragma once

#include "weapon.h"
#include "projectile.h"
#include "areaofeffect.h"
#include "game/application/application.h"
#include "projectilepool.h"

class ProjectileWeapon : public Weapon {
private:
    Projectile::Blueprint projectileBlueprint;

    bool onUse(const glm::ivec2& position, const glm::ivec2& target, bool isAnimationOnly);
    void apply(const glm::ivec2& position, const glm::ivec2& target);

public:
    ProjectileWeapon(
        Entity* owner,
        ApplicationContext* context,
        Item* item,
        EventPublisher<MeleeWeaponEventData>& publisher,
        const UUID& id,
        const std::string& name, 
        const WeaponStats2& stats,
        const DamageSource& damageSource,
        const Projectile::Blueprint& projectileBlueprint
    );

    ProjectileWeapon(
        Entity* owner,
        ApplicationContext* context,
        Item* item,
        EventPublisher<MeleeWeaponEventData>& publisher,
        const std::string& name, 
        const WeaponStats2& stats,
        const DamageSource& damageSource,
        const Projectile::Blueprint& projectileBlueprint
    );
    
    void update(int64_t timeSinceLastFrame);
    bool isAnimationInProgress(void);
    Type getType(void) const;

    bool isInRange(const glm::ivec2& target);

    void setProjectileBlueprint(const Projectile::Blueprint& projectileBlueprint);
    Projectile::Blueprint getProjectileBluePrint(void) const;
};