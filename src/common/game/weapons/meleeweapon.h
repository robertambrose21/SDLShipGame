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
        EventPublisher<WeaponEventData>& publisher,
        uint32_t id,
        const std::string& name, 
        const Stats& stats
    );

    MeleeWeapon(
        Entity* owner, 
        Grid* grid,
        EntityPool* entityPool,
        EventPublisher<WeaponEventData>& publisher,
        const std::string& name, 
        const Stats& stats
    );

    void update(int64_t timeSinceLastFrame);
    Type getType(void) const;
};