#include "areaofeffect.h"
#include "game/application/application.h"

AreaOfEffect::AreaOfEffect(
    Grid* grid,
    EntityPool* entityPool,
    EventPublisher<AreaOfEffectEventData>& publisher,
    uint32_t textureId,
    int ownerId,
    int liveTurn,
    bool isAnimationOnly,
    const glm::ivec2& position,
    const DamageSource& damageSource,
    const AoEStats& stats
) :
    grid(grid),
    entityPool(entityPool),
    publisher(publisher),
    textureId(textureId),
    ownerId(ownerId),
    liveTurn(liveTurn),
    isAnimationOnly(isAnimationOnly),
    position(position),
    damageSource(damageSource),
    stats(stats)
{
    effectedTilePositions = grid->getTilesInCircle(position.x, position.y, stats.radius);

    if(!isAnimationOnly) {
        apply();
    }
}

void AreaOfEffect::update(int64_t timeSinceLastFrame) {
    //
}

void AreaOfEffect::apply(void) {
    auto entities = entityPool->getEntities();
    auto effectedEntities = Entity::filterByTiles(effectedTilePositions, entities, ownerId);

    for(auto const& entity : effectedEntities) {
        publisher.publish<AreaOfEffectEventData>({ this, entity, damageSource.apply(entity) });
    }
}

void AreaOfEffect::onNextTurn(int currentParticipantId, int turnNumber) {
    if(isAnimationOnly) {
        return;
    }

    if(currentParticipantId != ownerId) {
        return;
    }

    apply();
}

int AreaOfEffect::getOwnerId(void) const {
    return ownerId;
}

AoEStats AreaOfEffect::getStats(void) const {
    return stats;
}

uint32_t AreaOfEffect::getTextureId(void) const {
    return textureId;
}

glm::ivec2 AreaOfEffect::getPosition(void) const {
    return position;
}

std::vector<glm::ivec2> AreaOfEffect::getEffectedTilePositions(void) {
    return effectedTilePositions;
}