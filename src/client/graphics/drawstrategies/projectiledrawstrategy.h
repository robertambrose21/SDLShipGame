#pragma once

#include "drawstrategy.h"
#include "game/weapons/projectile.h"

class ProjectileDrawStrategy : public DrawStrategy<Projectile> {
public:
    void draw(Projectile* drawable, GraphicsContext& graphicsContext);
};
