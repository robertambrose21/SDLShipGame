#include "itemdrawstrategy.h"

void ItemDrawStrategy::draw(Item* drawable, GraphicsContext& graphicsContext) {
    graphicsContext.getGridRenderer().draw(graphicsContext, drawable->getTextureId(), drawable->getPosition());
}