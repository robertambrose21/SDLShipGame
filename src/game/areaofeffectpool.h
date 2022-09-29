#pragma once

#include <vector>

#include "areaofeffect.h"

class AreaOfEffectPool {
private:
    std::vector<std::shared_ptr<AreaOfEffect>> aoeObjects;
    std::vector<std::shared_ptr<AreaOfEffect>> aoeObjectsForDeletion;

public:
    AreaOfEffectPool();

    void add(std::shared_ptr<AreaOfEffect> areaOfEffect);

    void draw(std::shared_ptr<SDL_Renderer> renderer);
    void update(const Uint32& timeSinceLastFrame);
};