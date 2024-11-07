#pragma once

#include "weapon.h"
#include "game/application/application.h"

class MeleeWeapon : public Weapon {
private:
    bool onUse(const glm::ivec2& position, const glm::ivec2& target, bool isAnimationOnly);
    void apply(const glm::ivec2& position, const glm::ivec2& target);

public:
    MeleeWeapon(
        Entity* owner, 
        ApplicationContext* context,
        Item* item,
        EventPublisher<MeleeWeaponEventData>& publisher,
        const UUID& id,
        const std::string& name,
        const DamageSource& damageSource,
        const WeaponStats2& stats
    );

    MeleeWeapon(
        Entity* owner, 
        ApplicationContext* context,
        Item* item,
        EventPublisher<MeleeWeaponEventData>& publisher,
        const std::string& name,
        const DamageSource& damageSource, 
        const WeaponStats2& stats
    );

    void update(int64_t timeSinceLastFrame);
    Type getType(void) const;
};