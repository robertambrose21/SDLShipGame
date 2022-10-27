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
        std::shared_ptr<Grid> grid, 
        uint8_t textureId,
        int liveTurn,
        glm::ivec2 position, 
        Stats stats
    );

    void draw(std::shared_ptr<GraphicsContext> graphicsContext);
    void update(const Uint32& timeSinceLastFrame);

    Stats getStats(void) const;
};