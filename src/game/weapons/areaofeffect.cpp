#include "areaofeffect.h"

AreaOfEffect::AreaOfEffect(
    const std::shared_ptr<Grid>& grid,
    uint32_t textureId,
    int liveTurn,
    const glm::ivec2& position, 
    const Stats& stats
) :
    grid(grid),
    textureId(textureId),
    liveTurn(liveTurn),
    position(position),
    stats(stats)
{
    effectedTilePositions = grid->getTilesInCircle(position.x, position.y, stats.radius);
}

void AreaOfEffect::draw(const std::shared_ptr<GraphicsContext>& graphicsContext) {
    game_assert(graphicsContext != nullptr);
    for(auto const& position : effectedTilePositions) {
        graphicsContext->getGridRenderer()->draw(graphicsContext, textureId, position);
    }
}

void AreaOfEffect::update(uint32_t timeSinceLastFrame) {
    //
}

AreaOfEffect::Stats AreaOfEffect::getStats(void) const {
    return stats;
}