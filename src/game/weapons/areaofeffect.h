#pragma once

#include "core/glmimport.h"
#include "graphics/gridrenderer.h"

class AreaOfEffect {
public:
    typedef struct _stats {
        float radius;
        int turns;
    } Stats;

private:
    std::shared_ptr<Grid> grid;
    uint8_t textureId;

    std::vector<glm::ivec2> effectedTilePositions;

    int liveTurn;
    glm::ivec2 position;
    Stats stats;

public:
    AreaOfEffect(
        const std::shared_ptr<Grid>& grid, 
        const uint8_t& textureId,
        const int& liveTurn,
        const glm::ivec2& position, 
        const Stats& stats
    );

    void draw(const std::shared_ptr<GraphicsContext>& graphicsContext);
    void update(const uint32_t& timeSinceLastFrame);

    Stats getStats(void) const;
};