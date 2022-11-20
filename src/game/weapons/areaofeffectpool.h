#pragma once

#include <vector>

#include "areaofeffect.h"
#include "game/application/turncontroller.h"

class AreaOfEffectPool {
private:
    std::vector<std::pair<int, std::shared_ptr<AreaOfEffect>>> aoeObjects;
    std::vector<std::pair<int, std::shared_ptr<AreaOfEffect>>> aoeObjectsForDeletion;

    std::shared_ptr<TurnController> turnController;

public:
    AreaOfEffectPool(std::shared_ptr<TurnController> turnController);

    void add(std::shared_ptr<AreaOfEffect> areaOfEffect);

    void draw(std::shared_ptr<GraphicsContext> graphicsContext);
    void update(const uint32_t& timeSinceLastFrame);
};