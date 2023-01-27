#include "areaofeffect.h"
#include "game/application/application.h"

AreaOfEffect::AreaOfEffect(
    const std::shared_ptr<Grid>& grid,
    uint32_t textureId,
    int ownerId,
    int liveTurn,
    const glm::ivec2& position, 
    const Stats& stats
) :
    grid(grid),
    textureId(textureId),
    ownerId(ownerId),
    liveTurn(liveTurn),
    position(position),
    stats(stats)
{
    effectedTilePositions = grid->getTilesInCircle(position.x, position.y, stats.radius);

    apply();
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

void AreaOfEffect::apply(void) {
    auto entities = Application::getContext()->getEntityPool()->getEntities();
    auto effectedEntities = Entity::filterByTiles(effectedTilePositions, entities, ownerId);

    for(auto const& entity : effectedEntities) {
        entity->takeDamage(stats.damagePerTurn);
    }
}

void AreaOfEffect::onNextTurn(int currentParticipant, int turnNumber) {
    if(currentParticipant != ownerId) {
        return;
    }

    apply();
}

AreaOfEffect::Stats AreaOfEffect::getStats(void) const {
    return stats;
}