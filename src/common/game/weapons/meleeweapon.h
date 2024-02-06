#pragma once

#include "weapon.h"
#include "game/application/application.h"

class MeleeWeapon : public Weapon {
private:
    bool onUse(const glm::ivec2& position, const glm::ivec2& target);

public:
    MeleeWeapon(
        Entity* owner, 
        Grid* grid,
        EntityPool* entityPool,
        Item* item,
        EventPublisher<WeaponEventData>& publisher,
        const UUID& id,
        const std::string& name, 
        const Stats& stats,
        const DamageSource& damageSource
    );

    MeleeWeapon(
        Entity* owner, 
        Grid* grid,
        EntityPool* entityPool,
        Item* item,
        EventPublisher<WeaponEventData>& publisher,
        const std::string& name, 
        const Stats& stats,
        const DamageSource& damageSource
    );

    void update(int64_t timeSinceLastFrame);
    Type getType(void) const;
};