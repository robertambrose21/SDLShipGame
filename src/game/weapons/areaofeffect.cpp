#include "areaofeffect.h"
#include "game/application/application.h"

AreaOfEffect::AreaOfEffect(
    Grid& grid,
    EventPublisher<AreaOfEffectEventData>& publisher,
    uint32_t textureId,
    int ownerId,
    int liveTurn,
    const glm::ivec2& position, 
    const Stats& stats
) :
    grid(grid),
    publisher(publisher),
    textureId(textureId),
    ownerId(ownerId),
    liveTurn(liveTurn),
    position(position),
    stats(stats)
{
    effectedTilePositions = grid.getTilesInCircle(position.x, position.y, stats.radius);

    apply();
}

void AreaOfEffect::draw(GraphicsContext& graphicsContext) {
    for(auto const& position : effectedTilePositions) {
        graphicsContext.getGridRenderer().draw(graphicsContext, textureId, position);
    }
}

void AreaOfEffect::update(uint32_t timeSinceLastFrame) {
    //
}

void AreaOfEffect::apply(void) {
    auto entities = Application::getContext().getEntityPool().getEntities();
    auto effectedEntities = Entity::filterByTiles(effectedTilePositions, entities, ownerId);

    for(auto const& entity : effectedEntities) {
        entity->takeDamage(stats.damagePerTurn);
        publisher.publish({ this, entity });
    }
}

void AreaOfEffect::onNextTurn(int currentParticipant, int turnNumber) {
    if(currentParticipant != ownerId) {
        return;
    }

    apply();
}

int AreaOfEffect::getOwnerId(void) const {
    return ownerId;
}

AreaOfEffect::Stats AreaOfEffect::getStats(void) const {
    return stats;
}