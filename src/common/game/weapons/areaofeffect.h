#pragma once

#include "core/glmimport.h"
#include "game/entities/entity.h"
#include "core/event/eventpublisher.h"

class EntityPool;
class AreaOfEffect;

struct AreaOfEffectEventData {
    AreaOfEffect* aoe;
    Entity* target;
};

class AreaOfEffect {
public:
    typedef struct _stats {
        float radius;
        int turns;
        int damagePerTurn;
    } Stats;

private:
    Grid& grid;
    EventPublisher<AreaOfEffectEventData>& publisher;

    uint32_t textureId;

    std::vector<glm::ivec2> effectedTilePositions;

    int ownerId;
    int liveTurn;
    glm::ivec2 position;
    Stats stats;

public:
    AreaOfEffect(
        Grid& grid,
        EventPublisher<AreaOfEffectEventData>& publisher,
        uint32_t textureId,
        int ownerId,
        int liveTurn,
        const glm::ivec2& position, 
        const Stats& stats
    );

    void update(uint32_t timeSinceLastFrame);
    void apply(void);
    void onNextTurn(int currentParticipant, int turnNumber);

    int getOwnerId(void) const;
    Stats getStats(void) const;

    std::vector<glm::ivec2> getEffectedTilePositions(void);

    uint32_t getTextureId(void) const;
    glm::ivec2 getPosition(void) const;
};