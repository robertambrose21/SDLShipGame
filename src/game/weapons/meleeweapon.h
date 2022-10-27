#pragma once

#include "weapon.h"

class MeleeWeapon : public Weapon {
private:
    void onUse(glm::ivec2 position, std::shared_ptr<Entity> target);

public:
    MeleeWeapon(
        std::shared_ptr<Entity> owner, 
        std::shared_ptr<Grid> grid, 
        const std::string& name, 
        Stats stats
    );

    void draw(std::shared_ptr<GraphicsContext> graphicsContext);
    void update(const Uint32& timeSinceLastFrame);
};