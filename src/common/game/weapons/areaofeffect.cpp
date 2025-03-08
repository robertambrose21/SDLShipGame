#include "areaofeffect.h"
#include "game/application/application.h"

AreaOfEffect::AreaOfEffect(
    Grid* grid,
    EntityPool* entityPool,
    EventPublisher<AreaOfEffectEventData>& publisher,
    uint32_t textureId,
    int ownerId,
    // int liveTurn,
    bool isAnimationOnly,
    const glm::ivec2& position,
    const DamageSource& damageSource,
    const Stats::AoEStats& stats
) :
    grid(grid),
    entityPool(entityPool),
    publisher(publisher),
    textureId(textureId),
    ownerId(ownerId),
    // liveTurn(liveTurn),
    turnsLeft(stats.duration),
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
    if(turnsLeft <= 0) {
        spdlog::warn("Cannot apply AoE at ({}, {}) - no turns left", position.x, position.y);
        return;
    }

    auto entities = entityPool->getEntities();
    auto effectedEntities = Entity::filterByTiles(effectedTilePositions, entities, ownerId);

    for(auto const& entity : effectedEntities) {
        publisher.publish<AreaOfEffectEventData>({ this, entity, damageSource.apply(entity) });
    }
}

void AreaOfEffect::onNextTurn(int currentParticipantId, int turnNumber) {
    if(currentParticipantId != ownerId) {
        return;
    }

    turnsLeft--;

    if(isAnimationOnly) {
        return;
    }

    if(turnsLeft > 0) {
        apply();
    }
}

int AreaOfEffect::getOwnerId(void) const {
    return ownerId;
}

Stats::AoEStats AreaOfEffect::getStats(void) const {
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

bool AreaOfEffect::isComplete(void) {
    return turnsLeft <= 0;
}