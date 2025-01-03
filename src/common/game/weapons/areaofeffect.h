#pragma once

#include "core/glmimport.h"
#include "game/entities/entity.h"
#include "core/event/eventpublisher.h"
#include "game/event/events.h"
#include "damagesource.h"

class EntityPool;
class AreaOfEffect;

class AreaOfEffect {
private:
    Grid* grid;
    EntityPool* entityPool;
    EventPublisher<AreaOfEffectEventData>& publisher;

    uint32_t textureId;

    std::vector<glm::ivec2> effectedTilePositions;

    int ownerId;
    int liveTurn;
    bool isAnimationOnly;
    glm::ivec2 position;
    DamageSource damageSource;
    Stats::AoEStats stats;

public:
    AreaOfEffect(
        Grid* grid,
        EntityPool* entityPool,
        EventPublisher<AreaOfEffectEventData>& publisher,
        uint32_t textureId,
        int ownerId,
        int liveTurn,
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
};