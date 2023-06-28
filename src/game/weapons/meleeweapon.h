#pragma once

#include "weapon.h"
#include "game/application/application.h"

class MeleeWeapon : public Weapon {
private:
    bool onUse(const glm::ivec2& position, const std::shared_ptr<Entity>& target);
    bool onUse(const glm::ivec2& position, const glm::ivec2& target);

public:
    MeleeWeapon(
        const std::shared_ptr<Entity>& owner, 
        const std::shared_ptr<Grid>& grid,
        uint32_t id,
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
    void update(uint32_t timeSinceLastFrame);
    Type getType(void) const;
};