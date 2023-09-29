#include "projectiledrawstrategy.h"

void ProjectileDrawStrategy::draw(Projectile* drawable, GraphicsContext& graphicsContext) {
    graphicsContext.getGridRenderer().draw(graphicsContext, drawable->getTextureId(), drawable->getPosition());
}