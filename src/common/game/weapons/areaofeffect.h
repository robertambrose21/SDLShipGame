#pragma once

#include "core/glmimport.h"
#include "game/entities/entity.h"
#include "core/event/eventpublisher.h"
#include "game/event/events.h"
#include "damagesource.h"

class EntityPool;
class AreaOfEffect;

class AreaOfEffect {
public:
    static const int64_t RealTimeTick = 1000;

    AreaOfEffect(
        Grid* grid,
        EntityPool* entityPool,
        EventPublisher<AreaOfEffectEventData>& publisher,
        uint32_t textureId,
        int ownerId,
        // int liveTurn,
        // int turnsLeft,
        bool isAnimationOnly,
        const glm::ivec2& position,
        const DamageSource& damageSource,
        const Stats::AoEStats& stats
    );

    void update(int64_t timeSinceLastFrame);
    void apply(void);
    void onNextTurn(int currentParticipantId, int turnNumber);

    int getOwnerId(void) const;
    Stats::AoEStats getStats(void) const;

    std::vector<glm::ivec2> getEffectedTilePositions(void);

    uint32_t getTextureId(void) const;
    glm::ivec2 getPosition(void) const;
    bool isComplete(void);

    int64_t getAdhocDuration(void) const;
    int64_t getTimeSinceLastTick(void) const;
    bool hasElapsedAdhocDuration(void);

private:
    Grid* grid;
    EntityPool* entityPool;
    EventPublisher<AreaOfEffectEventData>& publisher;

    uint32_t textureId;

    std::vector<glm::ivec2> effectedTilePositions;

    int ownerId;
    // int liveTurn;
    uint8_t turnsLeft;
    bool isAnimationOnly;
    glm::ivec2 position;
    DamageSource damageSource;
    Stats::AoEStats stats;

    int64_t adhocDuration;
    int64_t elapsedTime;
    int64_t timeSinceLastTick;
};