#pragma once

#include "game/actors/actor.h"
#include "drawstrategy.h"
#include "weapondrawstrategy.h"

class ActorDrawStrategy : public DrawStrategy<Actor> {
public:
    WeaponDrawStrategy* weaponDrawStrategy;

    ActorDrawStrategy(WeaponDrawStrategy* weaponDrawStrategy);

    void draw(Actor* drawable, GraphicsContext& graphicsContext);

private:
    void drawHealthBar(
        GraphicsContext& graphicsContext, 
        const glm::ivec2& position,
        int totalHP,
        int currentHP
    );
};
