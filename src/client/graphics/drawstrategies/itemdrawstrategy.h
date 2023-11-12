#pragma once

#include "drawstrategy.h"
#include "game/items/item.h"

class ItemDrawStrategy : public DrawStrategy<Item> {
public:
    void draw(Item* drawable, GraphicsContext& graphicsContext);
};
