#pragma once

#include "core/glmimport.h"
#include "graphics/gridrenderer.h"
#include "game/entities/entity.h"

class EntityPool;

class AreaOfEffect {
public:
    typedef struct _stats {
        float radius;
        int turns;
        int damagePerTurn;
    } Stats;

private:
    Grid& grid;
    uint32_t textureId;

    std::vector<glm::ivec2> effectedTilePositions;

    int ownerId;
    int liveTurn;
    glm::ivec2 position;
    Stats stats;

public:
    AreaOfEffect(
        Grid& grid, 
        uint32_t textureId,
        int ownerId,
        int liveTurn,
        const glm::ivec2& position, 
        const Stats& stats
    );

    void draw(GraphicsContext& graphicsContext);
    void update(uint32_t timeSinceLastFrame);
    void apply(void);
    void onNextTurn(int currentParticipant, int turnNumber);

    Stats getStats(void) const;
};