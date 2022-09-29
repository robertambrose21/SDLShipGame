#pragma once

#include <vector>

#include "areaofeffect.h"
#include "entitypool.h"

class AreaOfEffectPool {
private:
    std::vector<std::pair<int, std::shared_ptr<AreaOfEffect>>> aoeObjects;
    std::vector<std::pair<int, std::shared_ptr<AreaOfEffect>>> aoeObjectsForDeletion;

    std::shared_ptr<EntityPool> entityPool;

public:
    AreaOfEffectPool(std::shared_ptr<EntityPool> entityPool);

    void add(std::shared_ptr<AreaOfEffect> areaOfEffect);

    void draw(std::shared_ptr<SDL_Renderer> renderer);
    void update(const Uint32& timeSinceLastFrame);
};