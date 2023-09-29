#pragma once

#include "game/entities/entity.h"
#include "drawstrategy.h"
#include "weapondrawstrategy.h"

class EntityDrawStrategy : public DrawStrategy<Entity> {
public:
    WeaponDrawStrategy* weaponDrawStrategy;

    EntityDrawStrategy(WeaponDrawStrategy* weaponDrawStrategy);

    void draw(Entity* drawable, GraphicsContext& graphicsContext);

private:
    void drawHealthBar(
        GraphicsContext& graphicsContext, 
        const glm::ivec2& position,
        int totalHP,
        int currentHP
    );
};
