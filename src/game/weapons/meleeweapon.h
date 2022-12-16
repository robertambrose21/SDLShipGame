#pragma once

#include "weapon.h"

class MeleeWeapon : public Weapon {
private:
    void onUse(const glm::ivec2& position, const std::shared_ptr<Entity>& target);

public:
    MeleeWeapon(
        const std::shared_ptr<Entity>& owner, 
        const std::shared_ptr<Grid>& grid,
        const uint32_t& id,
        const std::string& name, 
        const Stats& stats
    );

    MeleeWeapon(
        const std::shared_ptr<Entity>& owner, 
        const std::shared_ptr<Grid>& grid, 
        const std::string& name, 
        const Stats& stats
    );

    void draw(const std::shared_ptr<GraphicsContext>& graphicsContext);
    void update(const uint32_t& timeSinceLastFrame);
    Type getType(void) const;
};