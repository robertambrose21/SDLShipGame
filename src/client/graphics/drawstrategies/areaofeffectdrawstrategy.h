#pragma once

#include "drawstrategy.h"
#include "game/weapons/areaofeffect.h"

class AreaOfEffectDrawStrategy : public DrawStrategy<AreaOfEffect> {
public:
    void draw(AreaOfEffect* drawable, GraphicsContext& graphicsContext);
};
