#include "areaofeffect.h"

AreaOfEffect::AreaOfEffect(
    std::shared_ptr<Grid> grid,
    uint8_t textureId,
    int liveTurn,
    glm::ivec2 position, 
    Stats stats
) :
    grid(grid),
    textureId(textureId),
    liveTurn(liveTurn),
    position(position),
    stats(stats)
{
    effectedTilePositions = grid->getTilesInCircle(position.x, position.y, stats.radius);
}

void AreaOfEffect::draw(std::shared_ptr<GraphicsContext> graphicsContext) {
    for(auto position : effectedTilePositions) {
        graphicsContext->getGridRenderer()->draw(graphicsContext, textureId, position);
    }
}

void AreaOfEffect::update(const Uint32& timeSinceLastFrame) {
    //
}

AreaOfEffect::Stats AreaOfEffect::getStats(void) const {
    return stats;
}