#include "areaofeffect.h"
#include "game/application/application.h"

AreaOfEffect::AreaOfEffect(
    Grid* grid,
    ActorPool* actorPool,
    EventPublisher<AreaOfEffectEventData>& publisher,
    uint32_t textureId,
    int ownerId,
    bool isAnimationOnly,
    const glm::ivec2& position,
    const DamageSource& damageSource,
    const Stats::AoEStats& stats
) :
    grid(grid),
    actorPool(actorPool),
    publisher(publisher),
    textureId(textureId),
    ownerId(ownerId),
    turnsLeft(stats.duration),
    isAnimationOnly(isAnimationOnly),
    position(position),
    damageSource(damageSource),
    stats(stats),
    adhocDuration(stats.duration * RealTimeTick),
    elapsedTime(0),
    timeSinceLastTick(0)
{
    effectedTilePositions = grid->getTilesInCircle(position.x, position.y, stats.radius);

    if(!isAnimationOnly) {
        apply();
    }
}

void AreaOfEffect::update(int64_t timeSinceLastFrame) {
    elapsedTime += timeSinceLastFrame;
    timeSinceLastTick += timeSinceLastFrame;
}

void AreaOfEffect::apply(void) {
    if(turnsLeft < 0) {
        spdlog::warn("Cannot apply AoE at ({}, {}) - no turns left", position.x, position.y);
        return;
    }

    auto actors = actorPool->getActors();
    auto effectedActors = Actor::filterByTiles(effectedTilePositions, actors, ownerId);

    spdlog::trace("AoE applied at ({}, {}), {} turns left", position.x, position.y, turnsLeft);

    for(auto const& actor : effectedActors) {
        publisher.publish<AreaOfEffectEventData>({ this, actor, damageSource.apply(actor) });
    }
}

void AreaOfEffect::onNextTurn(int currentParticipantId, int turnNumber) {
    if(currentParticipantId != ownerId) {
        return;
    }

    turnsLeft--;
    timeSinceLastTick = 0;

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

int64_t AreaOfEffect::getAdhocDuration(void) const {
    return adhocDuration;
}

int64_t AreaOfEffect::getTimeSinceLastTick(void) const {
    return timeSinceLastTick;
}

bool AreaOfEffect::hasElapsedAdhocDuration(void) {
    return elapsedTime > adhocDuration;
}
