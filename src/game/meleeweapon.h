#pragma once

#include "weapon.h"

class MeleeWeapon : public Weapon {
private:
    void onUse(glm::ivec2 position, std::shared_ptr<Entity> target);

public:
    MeleeWeapon(std::shared_ptr<GridRenderer> grid, const std::string& name, Stats stats);

    void draw(std::shared_ptr<SDL_Renderer> renderer);
    void update(const Uint32& timeSinceLastFrame);
};