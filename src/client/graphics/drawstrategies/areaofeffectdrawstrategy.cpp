#include "areaofeffectdrawstrategy.h"

void AreaOfEffectDrawStrategy::draw(AreaOfEffect* drawable, GraphicsContext& graphicsContext) {
    for(auto const& position : drawable->getEffectedTilePositions()) {
        graphicsContext.getGridRenderer().draw(graphicsContext, drawable->getTextureId(), position);
    }
}